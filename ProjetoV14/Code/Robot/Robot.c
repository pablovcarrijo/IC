#include "Robot.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int init_robots(int id, int rows, int columns, Robot *robot)
{
    robot->mapRobot = (int **)malloc(rows * sizeof(int *));
    robot->pheromoneGrade = (Cell **)malloc(rows * sizeof(Cell *));
    robot->detectionRadius = MOVE_RADIUS;
    robot->id = id;

    robot->link_list = (Robot_link *)malloc(sizeof(Robot_link));
    if (robot->link_list == NULL)
    {
        return 0;
    }
    robot->link_list->id = -1;
    robot->link_list->check = 0;
    robot->link_list->time_stamp = 0;
    robot->link_list->prox = NULL;

    for (int i = 0; i < rows; i++)
    {
        robot->pheromoneGrade[i] = (Cell *)malloc(columns * sizeof(Cell));
        (*robot).mapRobot[i] = (int *)malloc(columns * sizeof(int));

        for (int j = 0; j < columns; j++)
        {
            robot->pheromoneGrade[i][j].pos.x = i;
            robot->pheromoneGrade[i][j].pos.y = j;
            robot->pheromoneGrade[i][j].pheromoneValue = 0;
            robot->mapRobot[i][j] = CELL_UNKNOWN;
        }
    }
    return 1;
}

void place_robot_random(int rows, int columns, int quantityPlaced, Robot *robots, Robot *robot, RealMap *realMap)
{
    int x, y;

    do
    {
        x = (rand() % (rows - 2)) + 1;
        y = (rand() % (columns - 2)) + 1;
    } while (realMap->value[x][y] == CELL_WALL || verify_position_occupied(robots, quantityPlaced, x, y));
    robot->pos.x = x;
    robot->pos.y = y;

    for (int i = -MOVE_RADIUS; i <= MOVE_RADIUS; i++)
    {
        for (int j = -MOVE_RADIUS; j <= MOVE_RADIUS; j++)
        {
            int x = robot->pos.x + i;
            int y = robot->pos.y + j;

            if (x >= 0 && y >= 0 && x < rows && y < columns)
            {
                robot->mapRobot[x][y] = realMap->value[x][y];
            }
        }
    }

    robot->mapRobot[robot->pos.x][robot->pos.y] = CELL_ROBOT;
}

int verify_position_occupied(Robot *robots, int quantityPlaced, int x, int y)
{
    for (int i = 0; i < quantityPlaced; i++)
    {
        if (robots[i].pos.x == x && robots[i].pos.y == y)
        {
            return 1;
        }
    }
    return 0;
}

void robot_move(int rows, int columns, Robot *robot, Pool *pool, int id, RealMap *realMap)
{

    double max_phero_cel = 1.0; // max of pheromone per cell
    double alpha = 0.5;         // maximum possible amount of deposited pheromone
    double eta = 2.0;           // compensation of the environment evaporation rate
    double euler = 2.718281;    // Euler
    double delta = 0.1;         // influence of the pheromone deposition rate
    double PI = 3.141592;       // PI value
    int r = 1;                  // Distance of the cell {i,j} from the robot {r E N | 0 <= r <= rd}

    int neighbors_i[MAX_NEIGHBORS], neighbors_j[MAX_NEIGHBORS], neighbor_i, neighbor_j, n = 0;
    double vectWeight[MAX_NEIGHBORS], sum = 0.0, contribution = 0.5, acumulated = 0.0, randomNumber;
    int new_i, new_j, i, j;

    for (i = -MOVE_RADIUS; i <= MOVE_RADIUS; i++)
    {
        for (j = -MOVE_RADIUS; j <= MOVE_RADIUS; j++)
        {
            if (j == 0 && i == 0)
                continue;

            neighbor_i = robot->pos.x + i;
            neighbor_j = robot->pos.y + j;

            if (neighbor_i >= 0 && neighbor_j >= 0 && neighbor_i < rows && neighbor_j < columns)
            {
                robot->mapRobot[neighbor_i][neighbor_j] = realMap->value[neighbor_i][neighbor_j];

                if ((robot->mapRobot[neighbor_i][neighbor_j] != CELL_WALL) && (free_way(robot, robot->pos.x, robot->pos.y, neighbor_i, neighbor_j)))
                {

                    double psi_t = robot->pheromoneGrade[neighbor_i][neighbor_j].pheromoneValue;

                    // Deposit pheromone on neighboring cells
                    double deltaPsi = (max_phero_cel - psi_t) * // Calc of new phremone around the robot
                                      (alpha * pow(delta * euler, (eta) * (r / PI)));

                    robot->pheromoneGrade[neighbor_i][neighbor_j].pheromoneValue += deltaPsi; // Update the pheromone value around the robot

                    neighbors_i[n] = neighbor_i;
                    neighbors_j[n] = neighbor_j;
                    vectWeight[n] = robot->pheromoneGrade[neighbor_i][neighbor_j].pheromoneValue;
                    n++;
                }
            }
        }
    }

    robot->pheromoneGrade[robot->pos.x][robot->pos.y].pheromoneValue +=
        ((1.0 - robot->pheromoneGrade[robot->pos.x][robot->pos.y].pheromoneValue) * (1.3 * contribution));

    for (i = 0; i < n; i++)
    {
        vectWeight[i] = 1.0 - vectWeight[i];
        sum += vectWeight[i];
    }

    for (i = 0; i < n; i++)
    {
        vectWeight[i] = vectWeight[i] / sum;
    }

    randomNumber = ((double)rand()) / RAND_MAX;
    for (i = 0; i < n; i++)
    {
        acumulated += vectWeight[i];
        if (acumulated >= randomNumber)
        {
            break;
        }
    }
    i = (i < n) ? i : i - 1;

    int target_i = neighbors_i[i];
    int target_j = neighbors_j[i];

    int step_i = get_step_direction(target_i, robot->pos.x);
    int step_j = get_step_direction(target_j, robot->pos.y);

    new_i = robot->pos.x + step_i;
    new_j = robot->pos.y + step_j;

    if (new_i >= 0 && new_j >= 0 && new_i < rows && new_j < columns && robot->mapRobot[new_i][new_j] != CELL_WALL)
    {
        robot->mapRobot[robot->pos.x][robot->pos.y] = CELL_VISITED;
        robot->mapRobot[new_i][new_j] = CELL_ROBOT;

        robot->pos.x = new_i;
        robot->pos.y = new_j;
    }
}

int free_way(Robot *robot, int current_i, int current_j, int new_i, int new_j)
{
    int di = new_i - current_i;
    int dj = new_j - current_j;

    int steps = abs(di) >= abs(dj) ? abs(di) : abs(dj);

    if (steps == 0)
    {
        return 1;
    }

    for (int p = 1; p <= steps; p++)
    {
        int i = current_i + (di * p) / steps;
        int j = current_j + (dj * p) / steps;

        if (robot->mapRobot[i][j] == CELL_WALL)
        {
            return 0;
        }
    }

    return 1;
}

int get_step_direction(int target, int current)
{
    if (target > current)
    {
        return 1;
    }

    if (target < current)
    {
        return -1;
    }

    return 0;
}

void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            robot->pheromoneGrade[i][j].pheromoneValue -=
                beta * robot->pheromoneGrade[i][j].pheromoneValue;

            if (robot->pheromoneGrade[i][j].pheromoneValue < 0.0)
            {
                robot->pheromoneGrade[i][j].pheromoneValue = 0.0;
            }
        }
    }
}

void free_robot(Robot *robot, int rows)
{
    if (robot == NULL)
    {
        return;
    }

    if (robot->mapRobot != NULL)
    {
        for (int i = 0; i < rows; i++)
        {
            free(robot->mapRobot[i]);
        }

        free(robot->mapRobot);
    }

    if (robot->pheromoneGrade != NULL)
    {
        for (int i = 0; i < rows; i++)
        {
            free(robot->pheromoneGrade[i]);
        }

        free(robot->pheromoneGrade);
    }

    if (robot->link_list != NULL)
    {
        free(robot->link_list);
    }
}