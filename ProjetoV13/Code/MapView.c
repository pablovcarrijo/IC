#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "MapView.h"
#include "Communication.h"

void view_all_robot_real_map(int rows, int columns, int quantityRobots, Robot *robots, Pool *pool)
{
    system("cls");

    int cell, value, robotFound;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {

            robotFound = 0;
            for (int k = 0; k < quantityRobots; k++)
            {
                if (robots[k].pos.x == i && robots[k].pos.y == j)
                {
                    robotFound = 1;
                    continue;
                }
            }
            if (robotFound == 1)
            {
                printf("\033[48;2;0;0;255m  \033[m");
            }
            else
            {
                cell = CELL_UNKNOWN;
                for (int k = 0; k < quantityRobots; k++)
                {
                    value = robots[k].mapRobot[i][j];
                    if(value == CELL_WALL){
                        cell = CELL_WALL;
                        k = quantityRobots;
                    }
                    else if(value == CELL_VISITED){
                        cell = CELL_VISITED;
                    }
                    else if(value == CELL_FREE && cell == CELL_UNKNOWN){
                        cell = CELL_FREE;
                    }
                }

                if (cell == CELL_WALL)
                {
                    printf("\033[48;2;255;0;0m  \033[m");
                }
                else if (cell == CELL_VISITED || cell == CELL_FREE)
                {
                    printf("\033[48;2;0;255;0m  \033[m");
                }
                else if (cell == CELL_UNKNOWN)
                {
                    printf("\033[48;2;0;0;0m  \033[m");
                }
            }
        }
        printf("\n");
    }
    Sleep(200);
}

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

    Sleep(1000);
}

void view_real_robot_map(int rows, int columns, Robot *robot, Pool *pool)
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
            else if (robot->mapRobot[i][j] == CELL_WALL)
            {
                printf("\033[48;2;255;0;0m  \033[m");
            }
            else if (robot->mapRobot[i][j] == CELL_VISITED || robot->mapRobot[i][j] == CELL_FREE)
            {
                printf("\033[48;2;0;255;0m  \033[m");
            }
            else if (robot->mapRobot[i][j] == CELL_UNKNOWN)
            {
                printf("\033[48;2;0;0;0m  \033[m");
            }
        }
        printf("\n");
    }
    Sleep(300);
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
