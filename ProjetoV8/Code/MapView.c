#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "MapView.h"
#include "Communication.h"

void view_pheromone_robot_map(int rows, int columns, Robot *robot, Pool *pool)
{
    system("cls");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (robot->mapRobot[i][j] == CELL_ROBOT)
            {
                printf("\033[48;2;0;0;255m  \033[m");
            }
            else if (robot->mapRobot[i][j] != CELL_WALL)
            {
                int intensity = (int)(robot->pheromoneGrade[i][j].pheromoneValue * 255);

                printf("\033[48;2;0;%d;0m  \033[m", intensity);
            }
            else
            {
                printf("\033[48;2;255;0;0m  \033[m");
            }
        }
        printf("\n");
    }

    Sleep(600);
}

void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            robot->pheromoneGrade[i][j].pheromoneValue -= beta * robot->pheromoneGrade[i][j].pheromoneValue;
        }
    }
}
