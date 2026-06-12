#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "Robot.h"
#include "Communication.h"

int init_robots(int id, int rows, int columns, Robot *robot)
{
    robot->mapRobot = (int **)malloc(rows * sizeof(int *));
    robot->pheromoneGrade = (Cell **)malloc(rows * sizeof(Cell *));
    robot->detectionRadius = DETECTION_PHEROMONE_RADIUS;
    robot->id = id;
    
    
    robot->link_list = (Robot_link *)malloc(sizeof(Robot_link));
    if(robot->link_list == NULL){
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
        }
    }
    return 1;
}

void place_robot_random(int rows, int columns, Robot *robot)
{
    int x, y;

    do
    {
        x = (rand() % (rows - 2)) + 1;
        y = (rand() % (columns - 2)) + 1;
    } while (robot->mapRobot[x][y] == CELL_WALL);

    robot->pos.x = x;
    robot->pos.y = y;
    robot->mapRobot[x][y] = CELL_ROBOT;
}

void robot_move(int rows, int columns, Robot *robot, Pool *pool, int id)
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

    for (i = -DETECTION_PHEROMONE_RADIUS; i <= DETECTION_PHEROMONE_RADIUS; i++)
    {
        for (j = -DETECTION_PHEROMONE_RADIUS; j <= DETECTION_PHEROMONE_RADIUS; j++)
        {
            if (j == 0 && i == 0)
                continue;

            neighbor_i = robot->pos.x + i;
            neighbor_j = robot->pos.y + j;

            if (neighbor_i >= 0 && neighbor_j >= 0 && neighbor_i < rows && neighbor_j < columns)
            {
                if (robot->mapRobot[neighbor_i][neighbor_j] != CELL_WALL)
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
    new_i = neighbors_i[i];
    new_j = neighbors_j[i];

    robot->mapRobot[robot->pos.x][robot->pos.y] = CELL_VISITED;
    robot->mapRobot[new_i][new_j] = CELL_ROBOT;

    robot->pos.x = new_i;
    robot->pos.y = new_j;
}
