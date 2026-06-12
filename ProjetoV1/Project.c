#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "Project.h"

void init_map(int rows, int columns, Map *grade)
{

    grade->robot = NULL;
    grade->robotCount = 0;
    
    (*grade).mapGrade = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        (*grade).mapGrade[i] = (int *)malloc(columns * sizeof(int));
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            (*grade).mapGrade[i][j] = 0;
        }
    }
}

void robotsMap(int quantity, int rows, int columns, Map *map)
{
    int positionX, positionY;

    map->robot = (Robot *)malloc(quantity * sizeof(Robot));
    map->robotCount = quantity;
    srand(time(NULL));

    for (int r = 0; r < quantity; r++)
    {
        map->robot[r].pheromoneGrade = (double **)malloc(rows * sizeof(double *));
        for (int i = 0; i < rows; i++)
        {
            map->robot[r].pheromoneGrade[i] = (double *)malloc(columns * sizeof(double));
            for (int j = 0; j < columns; j++)
            {
                if (i == 0 || i == rows - 1 || j == 0 || j == columns - 1)
                {
                    map->robot[r].pheromoneGrade[i][j] = 10000;
                }
                else
                {
                    map->robot[r].pheromoneGrade[i][j] = 0;
                }
            }
        }

        do
        {
            positionX = (rand() % (rows - 2)) + 1;
            positionY = (rand() % (columns - 2)) + 1;
        } while (map->mapGrade[positionX][positionY] != 0);

        map->robot[r].id = r + 1;
        map->robot[r].pos.x = positionX;
        map->robot[r].pos.y = positionY;
        map->mapGrade[positionX][positionY] = 2;
    }
}

// void robotLocale(int rows, int columns, Map grade, Position *pos){
//     for(int i = 0; i < rows; i++){
//         for(int j = 0; j < columns; j++){
//             if(grade.mapGrade[i][j] == 2){
//                 pos->i = i;
//                 pos->j = j;
//                 break;
//             }
//         }
//     }
// }

void robotMove(int rows, int columns, Map *grade)
{

    double max_phero_cel = 1.0; // max of pheromone per cell
    double alpha = 0.5;         // maximum possible amount of deposited pheromone
    double eta = 2.0;           // compensation of the environment evaporation rate
    double euler = 2.718281;    // Euler
    double delta = 0.1;         // influence of the pheromone deposition rate
    double PI = 3.141592;       // PI value
    int r = 1;                  // Distance of the cell {i,j} from the robot {r E N | 0 <= r <= rd}

    for (int l = 0; l < grade->robotCount; l++)
    {
        int neighbors_i[8], neighbors_j[8], neighbor_i, neighbor_j, n = 0;
        double vectWeight[8], sum = 0.0, contribution = 0.5, acumulated = 0.0, randomNumber;
        int new_i, new_j, i, j;

        for (i = -1; i <= 1; i++)
        {
            for (j = -1; j <= 1; j++)
            {
                if (j == 0 && i == 0)
                    continue;

                neighbor_i = grade->robot[l].pos.x + i;
                neighbor_j = grade->robot[l].pos.y + j;

                if (neighbor_i >= 0 && neighbor_j >= 0 && neighbor_i < rows && neighbor_j < columns)
                {
                    if (grade->robot[l].pheromoneGrade[neighbor_i][neighbor_j] < 10000)
                    {

                        double psi_t = grade->robot[l].pheromoneGrade[neighbor_i][neighbor_j];

                        // Deposit pheromone on neighboring cells
                        double deltaPsi = (max_phero_cel - psi_t) * // Calc of new phremone around the robot
                                          (alpha * pow(delta * euler, (eta) * (r / PI)));

                        grade->robot[l].pheromoneGrade[neighbor_i][neighbor_j] += deltaPsi; // Update the pheromone value around the robot

                        neighbors_i[n] = neighbor_i;
                        neighbors_j[n] = neighbor_j;
                        vectWeight[n] = grade->robot[l].pheromoneGrade[neighbor_i][neighbor_j];
                        // sum += vectWeight[n++];
                        n++;
                    }
                }
            }
        }
        grade->robot[l].pheromoneGrade[grade->robot[l].pos.x][grade->robot[l].pos.y] += ((1.0 - grade->robot[l].pheromoneGrade[grade->robot[l].pos.x][grade->robot[l].pos.y]) * (1.3 * contribution));

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

        (*grade).mapGrade[grade->robot[l].pos.x][grade->robot[l].pos.y] = 1;
        (*grade).mapGrade[new_i][new_j] = 2;
        grade->robot[l].pos.x = new_i;
        grade->robot[l].pos.y = new_j;
    }
}

void viewMap(int rows, int columns, Map *map)
{
    system("cls");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (map->mapGrade[i][j] == 2)
                printf("R");
            else if (map->mapGrade[i][j] == 1)
                printf(".");
            else
                printf("  ");
        }
        printf("\n");
    }
    Sleep(400);
}

void viewRobotMap(int rows, int columns, Robot *robot)
{
    system("cls");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (robot->pheromoneGrade[i][j] < 10000)
            {
                int intensity = (int)(robot->pheromoneGrade[i][j] * 255);

                printf("\033[48;2;0;%d;0m  \033[m", intensity);
            }
            else
            {
                printf("\033[48;2;255;0;0m  \033[m");
            }
        }
        printf("\n");
    }
    Sleep(400);
}

void pheromone_evaporate(Robot *robot, int rows, int columns, double beta)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (robot->pheromoneGrade[i][j] < 10000)
            {
                robot->pheromoneGrade[i][j] -= beta * robot->pheromoneGrade[i][j];
            }
        }
    }
}