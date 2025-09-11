#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "Project.h"

void init_map(int rows, int columns, Map *grade)
{

    (*grade).mapGrade = (int **)malloc(rows * sizeof(int *));
    (*grade).pheromoneGrade = (double **)malloc(rows * sizeof(double *));
    grade->robot = NULL;
    grade->robotCount = 0;

    for (int i = 0; i < rows; i++)
    {
        (*grade).mapGrade[i] = (int *)malloc(columns * sizeof(int));
        (*grade).pheromoneGrade[i] = (double *)malloc(columns * sizeof(double));
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            (*grade).mapGrade[i][j] = 0;
            (*grade).pheromoneGrade[i][j] = 0.0;
            if (i == 0 || i == (rows - 1))
                (*grade).pheromoneGrade[i][j] = 10000;
            if (j == 0 || j == (columns - 1))
                (*grade).pheromoneGrade[i][j] = 10000;
        }
    }
}

void robotsMap(int quantity, int rows, int columns, Map *map)
{

    int positionX, positionY;

    map->robot = (Robot *)malloc(quantity * sizeof(Robot));
    map->robotCount = quantity;
    srand(time(NULL));

    for (int i = 0; i < quantity; i++)
    {
        do
        {
            positionX = rand() % rows;
            positionY = rand() % columns;
        } while ((*map).pheromoneGrade[positionX][positionY] >= 10000 || map->mapGrade[positionX][positionY] != 0);

        map->robot[i].pos.i = positionX;
        map->robot[i].pos.j = positionY;
        map->robot[i].id = (i + 1);

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

    double max_phero_cel = 1.0;   // max of pheromone per cell
    double alpha = 0.5;           // max of deposit
    double eta = 2.0; // compensation tax of evaporate pheromone enviroment
    double euler = 2.718281; // Euler
    double delta = 0.1; // influence of tax of deposit pheromone
    double PI = 3.141592;
    int r = 1;

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

                neighbor_i = grade->robot[l].pos.i + i;
                neighbor_j = grade->robot[l].pos.j + j;

                if (neighbor_i >= 0 && neighbor_j >= 0 && neighbor_i < rows && neighbor_j < columns)
                {
                    if ((*grade).pheromoneGrade[neighbor_i][neighbor_j] < 10000)
                    {

                        double psi_t = grade->pheromoneGrade[neighbor_i][neighbor_j];
                        
                        
                        
                        //Deposit pheromone on neighboring cells
                        double deltaPsi = (max_phero_cel - psi_t) * // Calc of new phremone around the robot
                                (alpha * pow(delta * euler, (eta) * (r/PI)));
                        
                        grade->pheromoneGrade[neighbor_i][neighbor_j] += deltaPsi; // Update the pheromone value around the robot

                        neighbors_i[n] = neighbor_i;
                        neighbors_j[n] = neighbor_j;
                        vectWeight[n] = (*grade).pheromoneGrade[neighbor_i][neighbor_j];
                        // sum += vectWeight[n++];
                        n++;
                    }
                    
                }
            }
            
        }
        grade->pheromoneGrade[grade->robot[l].pos.i][grade->robot[l].pos.j] += ((1.0 - (*grade).pheromoneGrade[grade->robot[l].pos.i][grade->robot[l].pos.j]) * (1.3 * contribution));

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

        (*grade).mapGrade[grade->robot[l].pos.i][grade->robot[l].pos.j] = 1;
        (*grade).mapGrade[new_i][new_j] = 2;
        grade->robot[l].pos.i = new_i;
        grade->robot[l].pos.j = new_j;
    }
}

void viewMap(int rows, int columns, Map grade)
{
    system("cls");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if ((grade).pheromoneGrade[i][j] < 10000)
            {
                printf("\033[48;2;0;%d;0m \033[m", (int)(grade.pheromoneGrade[i][j] * 255));
            }
            else
            {
                printf("\033[48;2;255;0;0m \033[m");
            }
        }
        printf("\n");
    }
    Sleep(400);
}

void pheromone_evaporate(int rows, int columns, Map *grade, int percentage, double beta)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if ((*grade).pheromoneGrade[i][j] < 10000)
            {
                (*grade).pheromoneGrade[i][j] =
                     ((*grade).pheromoneGrade[i][j] - (beta * (*grade).pheromoneGrade[i][j]));
            }
        }
    }
}