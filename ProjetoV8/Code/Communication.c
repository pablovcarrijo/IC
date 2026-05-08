#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
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
    if ((robot != NULL) && (id != NULL))
    {
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
int com_WriteOnPool(Robot *robot, Pool *pool)
{

    if ((robot != NULL) && (robot->pheromoneGrade != NULL) && (pool != NULL))
    {
        if (!com_poolVerifyCommunication(robot, pool))
        {
            com_poolNewCommunication(robot, pool);
        }
        com_poolUpdateCommunication(robot, pool);
        return 1;
    }

    return 0;
}

/*
    For each message in pool, verify is the robot can get that communication
    If he can, update his map with the most current value and update the hush time
*/
int com_ReadPool(Robot *robot, Pool *pool)
{

    Message *aux;
    long int aux_ts;
    int peer_position[2];
    int current_position[2];
    current_position[0] = robot->pos.x;
    current_position[1] = robot->pos.y;
    if ((robot != NULL) && (pool != NULL))
    {
        for (aux = pool->msgs; aux != NULL; aux = aux->prox)
        {
            if (aux->idRobot == robot->id)
            {
                continue;
            }

            peer_position[0] = aux->pos.x;
            peer_position[1] = aux->pos.y;

            if (com_inDetectionRange(robot, current_position, peer_position) &&
                com_verifyRobotLink(robot, aux->idRobot, aux->time_stamp))
            {
                printf("robot %d catch\n", robot->id);
                Sleep(1000);
                for (int k = 0; k < aux->pathMemory.size; k++)
                {
                    int index = (aux->pathMemory.front + k) % aux->pathMemory.capacity;

                    Cell c = aux->pathMemory.cells[index];

                    robot->pheromoneGrade[c.pos.x][c.pos.y].pheromoneValue =
                        fmax(robot->pheromoneGrade[c.pos.x][c.pos.y].pheromoneValue,
                             c.pheromoneValue);
                }
            }
        }
        com_computeLinkHush(robot);
        return 1;
    }
    return 0;
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

    node->pathMemory.capacity = QUEUE_MAX;
    node->pathMemory.size = 0;
    node->pathMemory.front = 0;
    node->pathMemory.tail = -1;

    node->pathMemory.cells = (Cell *)malloc(QUEUE_MAX * sizeof(Cell));

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
            ;
        aux->prox = node;
    }

    return 1;
}

// Update robot's pool, adding the new value on linked list msgs
int com_poolUpdateCommunication(Robot *robot, Pool *pool)
{
    int i, j;
    Message *aux;

    for (aux = pool->msgs; aux != NULL; aux = aux->prox)
    {
        if (robot->id == aux->idRobot)
        {
            Cell c;
            c.pos = robot->pos;
            
            for (i = -DETECTION_PHEROMONE_RADIUS; i <= DETECTION_PHEROMONE_RADIUS; i++){
                for(j = -DETECTION_PHEROMONE_RADIUS; j <= DETECTION_PHEROMONE_RADIUS; j++){
                    c.pos.x = robot->pos.x + i;
                    c.pos.y = robot->pos.y + j;
                    c.pheromoneValue = robot->pheromoneGrade[c.pos.x][c.pos.y].pheromoneValue;
                    enqueue_cell(&aux->pathMemory, c);
                    aux->pos = robot->pos;
                }
            }

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