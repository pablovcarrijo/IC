#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "loadMap.h"

int load_map_from_txt(const char *filePath, int rows, int columns, Robot *robot)
{
    FILE *f = fopen(filePath, "r");
    char line[2048];

    if (f == NULL)
    {
        return 0;
    }

    for (int i = 0; i < 2; i++)
    {
        if (fgets(line, sizeof(line), f) == NULL)
        {
            fclose(f);
            return 0;
        }
    }

    for (int i = 0; i < rows; i++)
    {
        if (fgets(line, sizeof(line), f) == NULL)
        {
            break;
        }

        for (int j = 0; j < columns && line[j] != '\0' && line[j] != '\n' && line[j] != '\r'; j++)
        {
            robot->mapRobot[i][j] = line[j] - '0';
            robot->pheromoneGrade[i][j].pheromoneValue = 0;
        }
    }

    fclose(f);
    return 1;
}

int get_map_size(char *filePath, int *rows, int *columns)
{
    FILE *f = fopen(filePath, "r");
    if (!f)
        return 0;

    char line[4096];

    for (int i = 0; i < 2; i++)
    {
        if (fgets(line, sizeof(line), f) == NULL)
        {
            fclose(f);
            return 0;
        }
    }

    *rows = 0;
    *columns = 0;

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = '\0';

        int len = strlen(line);

        if (len == 0)
            continue;

        if (*columns == 0)
        {
            *columns = len;
        }
        else if (len != *columns)
        {
            printf("Erro: linhas com tamanhos diferentes!\n");
            fclose(f);
            return 0;
        }

        (*rows)++;
    }

    fclose(f);
    return 1;
}

int get_map_cyclos_robots(char *filePath, int *cyclos, int *robots)
{
    FILE *pont_arq;

    pont_arq = fopen(filePath, "r");

    if (pont_arq == NULL)
    {
        printf("Erro ao tentar abrir o arquivo de entrada de dados\n");
        exit(1);
    }

    fscanf(pont_arq, "%d%*c", &(*cyclos));
    fscanf(pont_arq, "%d", &(*robots));

    fclose(pont_arq);
}

void init_queue(CellQueue *q, int capacity)
{
    q->cells = (Cell *)malloc(capacity * sizeof(Cell));
    q->capacity = capacity;
    q->front = 0;
    q->tail = -1;
    q->size = 0;
}