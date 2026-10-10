#include "LoadMap.h"
#include "../Config/Project.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **allocate_matrix(int rows, int cols, int initialValue)
{
    int **matrix = malloc(rows * sizeof(int *));

    if (matrix == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < rows; i++)
    {
        matrix[i] = malloc(cols * sizeof(int));

        if (matrix[i] == NULL)
        {
            for (int k = 0; k < i; k++)
            {
                free(matrix[k]);
            }

            free(matrix);
            return NULL;
        }

        for (int j = 0; j < cols; j++)
        {
            matrix[i][j] = initialValue;
        }
    }

    return matrix;
}

void free_matrix(int **matrix, int rows)
{
    if (matrix == NULL)
    {
        return;
    }

    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }

    free(matrix);
}

void remove_line_break(char *line)
{
    line[strcspn(line, "\r\n")] = '\0';
}

int load_map_from_txt(
    const char *filePath,
    int *maxCycles,
    int *quantityRobots,
    int *rows,
    int *cols,
    int ***map
)
{
    FILE *file = fopen(filePath, "r");

    if (file == NULL)
    {
        printf("Erro: nao foi possivel abrir o arquivo %s\n", filePath);
        return 0;
    }

    if (fscanf(file, "%d", maxCycles) != 1)
    {
        printf("Erro: nao foi possivel ler a quantidade de ciclos.\n");
        fclose(file);
        return 0;
    }

    if (fscanf(file, "%d", quantityRobots) != 1)
    {
        printf("Erro: nao foi possivel ler a quantidade de robos.\n");
        fclose(file);
        return 0;
    }

    char line[MAX_LINE_SIZE];

    fgets(line, sizeof(line), file);

    long mapStartPosition = ftell(file);

    *rows = 0;
    *cols = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        remove_line_break(line);

        int len = strlen(line);

        if (len == 0)
        {
            continue;
        }

        if (*cols == 0)
        {
            *cols = len;
        }
        else if (len != *cols)
        {
            printf("Erro: o mapa possui linhas com tamanhos diferentes.\n");
            fclose(file);
            return 0;
        }

        (*rows)++;
    }

    if (*rows == 0 || *cols == 0)
    {
        printf("Erro: mapa vazio ou invalido.\n");
        fclose(file);
        return 0;
    }

    *map = allocate_matrix(*rows, *cols, CELL_FREE);

    if (*map == NULL)
    {
        printf("Erro: falha ao alocar memoria para o mapa.\n");
        fclose(file);
        return 0;
    }

    fseek(file, mapStartPosition, SEEK_SET);

    int currentRow = 0;

    while (fgets(line, sizeof(line), file) != NULL && currentRow < *rows)
    {
        remove_line_break(line);

        int len = strlen(line);

        if (len == 0)
        {
            continue;
        }

        for (int j = 0; j < *cols; j++)
        {
            if (line[j] == '0')
            {
                (*map)[currentRow][j] = CELL_FREE;
            }
            else if (line[j] == '2')
            {
                (*map)[currentRow][j] = CELL_WALL;
            }
            else
            {
                printf("Erro: caractere invalido no mapa: %c\n", line[j]);
                free_matrix(*map, *rows);
                fclose(file);
                return 0;
            }
        }

        currentRow++;
    }

    fclose(file);
    return 1;
}