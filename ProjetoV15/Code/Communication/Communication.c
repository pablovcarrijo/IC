#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Communication.h"

/*
    Pool initializer, where pool->msgs = NULL and pool->size = size;
*/
int com_poolInit(Pool *pool, int size)
{

    if (pool == NULL)
    {
        return 0;
    }

    pool->msgs = NULL;
    pool->size = size;
    return 1;

    return 0;
}

// Set detection radius to robot
int com_setDetectionRadius(Robot *robot, int detectionRadius)
{
    if (robot != NULL)
    {
        robot->detectionRadius = detectionRadius;
        return 1;
    }
    return 0;
}

// Set id of one robot on liked list
int com_setIdRobotLink(Robot *robot, int id)
{
    Robot_link *aux;

    if (robot != NULL && id >= 0){
        aux = robot->link_list;
        if (aux->id != -1)
        {
            aux->id = -1;
        }
        aux->id = id;
        return 1;
    }
    return 0;
}

// Verify if two robots are in detection ranges
int com_inDetectionRange(Robot *robotA, int *a_center, int *b_center)
{
    int temp1, temp2;

    if (robotA->detectionRadius == -1)
    {
        return 1;
    }

    if ((a_center[0] < 0) || (a_center[1] < 0) || (b_center[0] < 0) || (b_center[1] < 0))
    {
        return 0;
    }

    temp1 = (int)fabs(a_center[0] - b_center[0]);
    temp2 = (int)fabs(a_center[1] - b_center[1]);
    return ((temp1 <= robotA->detectionRadius) && (temp2 <= robotA->detectionRadius)) ? 1 : 0;
}

// Verify if one robot is linked with another robot
int com_verifyRobotLink(Robot *robot, int id, long int time_stamp)
{
    Robot_link *aux;

    if (robot == NULL || robot->link_list == NULL)
    {
        return 0;
    }

    if (robot->link_list->id != id)
    {
        for (aux = robot->link_list->prox; aux != NULL; aux = aux->prox)
        {
            if (aux->id == id)
            {
                if (time_stamp > aux->time_stamp)
                {
                    aux->check = 1;
                    aux->time_stamp = time_stamp;
                    return 1;
                }
                return 0;
            }
        }
    }
    aux = (Robot_link *)malloc(sizeof(Robot_link));
    if (aux != NULL)
    {
        aux->id = id;
        aux->check = 1;
        aux->hush = 0;
        aux->time_stamp = time_stamp;

        aux->prox = robot->link_list->prox;
        robot->link_list->prox = aux;
        return 1;
    }

    return 0;
}

// Update the hush (time that the robot doesn't communicate )
int com_computeLinkHush(Robot *robot)
{
    Robot_link *aux;

    for (aux = robot->link_list->prox; aux != NULL; aux = aux->prox)
    {
        if (aux->check == 1)
        {
            aux->check = 0;
            aux->hush = 0;
        }
        else
        {
            aux->hush = ((aux->hush + 1) < 1000000) ? (aux->hush + 1) : aux->hush;
        }
    }
    return 1;
}

/*
    Write on Poll
        Verify if the robot communication with pool exists
            - If not exists, one communication is create and update
            - Else, just update the pool
*/
int com_WriteOnPool(Robot *robot, Pool *pool, int rows, int columns)
{

    if ((robot != NULL) && (robot->pheromoneGrade != NULL) && (pool != NULL))
    {
        if (!com_poolVerifyCommunication(robot, pool))
        {
            com_poolNewCommunication(robot, pool);
        }
        com_poolUpdateCommunication(robot, pool, rows, columns);
        return 1;
    }

    return 0;
}

/*
    For each message in pool, verify is the robot can get that communication with another robots
    If he can, update his map with the most current value and update the hush time
*/
int com_ReadPool(Robot *robot, Pool *pool, RealMap *realMap, int rows, int columns)
{
    if (robot == NULL || pool == NULL || realMap == NULL)
    {
        return 0;
    }

    Message *aux;
    int peer_position[2];
    int current_position[2];

    current_position[0] = robot->pos.x;
    current_position[1] = robot->pos.y;

    for (aux = pool->msgs; aux != NULL; aux = aux->prox)
    {
        if (aux->idRobot == robot->id)
        {
            continue;
        }

        peer_position[0] = aux->pos.x;
        peer_position[1] = aux->pos.y;

        if (com_inDetectionRange(robot, current_position, peer_position) &&
            com_hasLineOfSight(realMap, rows, columns,
                               current_position[0], current_position[1],
                               peer_position[0], peer_position[1]) &&
            com_verifyRobotLink(robot, aux->idRobot, aux->time_stamp))
        {
            for (int k = 0; k < aux->pathMemory.size; k++)
            {
                int index = (aux->pathMemory.front + k) % aux->pathMemory.capacity;

                Cell c = aux->pathMemory.cells[index];

                robot->pheromoneGrade[c.pos.x][c.pos.y].pheromoneValue =
                    fmax(robot->pheromoneGrade[c.pos.x][c.pos.y].pheromoneValue,
                         c.pheromoneValue);

                if (robot->mapRobot[c.pos.x][c.pos.y] != CELL_ROBOT)
                {
                    if (c.mapValue == CELL_UNKNOWN)
                    {
                        continue;
                    }

                    if (c.mapValue == CELL_ROBOT)
                    {
                        robot->mapRobot[c.pos.x][c.pos.y] = CELL_VISITED;
                    }
                    else
                    {
                        robot->mapRobot[c.pos.x][c.pos.y] = c.mapValue;
                    }
                }
            }
        }
    }

    com_computeLinkHush(robot);
    return 1;
}

// Checks if robot has a space in the pool
int com_poolVerifyCommunication(Robot *robot, Pool *pool)
{
    Message *aux;

    if (robot == NULL || pool == NULL)
    {
        return 0;
    }

    for (aux = pool->msgs; aux != NULL; aux = aux->prox)
    {
        if (aux->idRobot == robot->id)
        {
            return 1;
        }
    }

    return 0;
}

/*
    Create a new space in the pool for the robot
        If pool->msgs is NULL, that robot becomes the head of linked list
        Else, just add the robot on linked list
*/
int com_poolNewCommunication(Robot *robot, Pool *pool)
{

    int i;
    Message *node, *aux;

    node = (Message *)malloc(sizeof(Message));
    if (node == NULL)
    {
        return 0;
    }

    node->idRobot = robot->id;
    node->pos = robot->pos;
    node->time_stamp = 0;
    node->prox = NULL;

    node->pathMemory.capacity = MEMORY_QUEUE_MAX;
    node->pathMemory.size = 0;
    node->pathMemory.front = 0;
    node->pathMemory.tail = -1;

    node->pathMemory.cells = (Cell *)malloc(MEMORY_QUEUE_MAX * sizeof(Cell));

    if (node->pathMemory.cells == NULL)
    {
        return 0;
    }

    if (pool->msgs == NULL)
    {
        pool->msgs = node;
    }

    else
    {
        for (aux = pool->msgs; aux->prox != NULL; aux = aux->prox)
        {
        };
        aux->prox = node;
    }

    return 1;
}

// Update robot's pool, adding the new value on linked list msgs
int com_poolUpdateCommunication(Robot *robot, Pool *pool, int rows, int columns)
{
    int i, j;
    Message *aux;

    for (aux = pool->msgs; aux != NULL; aux = aux->prox)
    {
        if (robot->id == aux->idRobot)
        {

            for (i = -DETECTION_PHEROMONE_RADIUS; i <= DETECTION_PHEROMONE_RADIUS; i++)
            {
                for (j = -DETECTION_PHEROMONE_RADIUS; j <= DETECTION_PHEROMONE_RADIUS; j++)
                {
                    Cell c;
                    c.pos.x = robot->pos.x + i;
                    c.pos.y = robot->pos.y + j;
                    
                    if (c.pos.x < 0 || c.pos.x >= rows || c.pos.y < 0 || c.pos.y >= columns)
                    {
                        continue;
                    }

                    c.pheromoneValue = robot->pheromoneGrade[c.pos.x][c.pos.y].pheromoneValue;
                    c.mapValue = robot->mapRobot[c.pos.x][c.pos.y];
                    enqueue_cell(&aux->pathMemory, c);
                }
            }
            aux->pos = robot->pos;
            aux->time_stamp++;

            return 1;
        }
    }
    return 0;
}

// Auxiliary functions
void enqueue_cell(CellQueue *q, Cell cell)
{
    if (q->size == q->capacity)
    {
        q->front = (q->front + 1) % q->capacity;
        q->size--;
    }

    q->tail = (q->tail + 1) % q->capacity;
    q->cells[q->tail] = cell;
    q->size++;
}

void dequeue_cell(CellQueue *q)
{
    Cell c = q->cells[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
}

void print_queue(CellQueue *q)
{
    for (int i = 0; i < q->size; i++)
    {
        int index = (q->front + i) % q->capacity;
        printf("Cell %d = (%d, %d) pheromone: %.2f\n", i, q->cells[index].pos.x, q->cells[index].pos.y,
               q->cells[index].pheromoneValue);
    }
}

int com_hasLineOfSight(RealMap *realMap, int rows, int columns, int start_x, int start_y, int end_x, int end_y)
{
    if (realMap == NULL)
    {
        return 0;
    }

    int dx = end_x - start_x;
    int dy = end_y - start_y;

    int steps = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

    if (steps == 0)
    {
        return 1;
    }

    for (int p = 1; p < steps; p++)
    {
        int x = start_x + (dx * p) / steps;
        int y = start_y + (dy * p) / steps;

        if (x < 0 || y < 0 || x >= rows || y >= columns)
        {
            return 0;
        }

        if (realMap->value[x][y] == CELL_WALL)
        {
            return 0;
        }
    }

    return 1;
}