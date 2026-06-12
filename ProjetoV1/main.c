#include <stdio.h>
#include <stdlib.h>
#include "Project.h"

int main()
{

    Map grade;
    Position pos;
    int rows, columns, ciclos, quantityRobots, percentage = 20;

    FILE *pont_arq;

    pont_arq = fopen("entradas.txt", "r");

    if (pont_arq == NULL)
    {
        printf("Erro ao tentar abrir o arquivo de entrada de dados\n");
        exit(1);
    }

    fscanf(pont_arq, "%d%*c", &rows);
    fscanf(pont_arq, "%d%*c", &columns);
    fscanf(pont_arq, "%d%*c", &ciclos);
    fscanf(pont_arq, "%d", &quantityRobots);

    fclose(pont_arq);

    init_map(rows, columns, &grade);
    robotsMap(quantityRobots, rows, columns, &grade);

    for (int i = 0; i < ciclos; i++)
    {
        //viewMap(rows, columns, &grade); // Visualizo o mapa todo com todos os robôs
        viewRobotMap(rows, columns, &grade.robot[0]); // Visualizo apenas um robô
        robotMove(rows, columns, &grade);
        for (int j = 0; j < grade.robotCount; j++)
        {
            pheromone_evaporate(&grade.robot[j], rows, columns, 0.005);
        }
    }

    return 0;
}