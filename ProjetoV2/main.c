#include <stdio.h>
#include <stdlib.h>
#include "Project.h"

int main()
{

    Map grade;
    int rows, columns, ciclos, quantityRobots;

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
    load_map_from_txt("entradas.txt", rows, columns, &grade);

    robotsMap(quantityRobots, rows, columns, &grade);

    for (int i = 0; i < ciclos; i++)
    {
        // viewMap(rows, columns, &grade); // Visualizo o mapa todo com todos os robôs
        // viewRobotMap(rows, columns, &grade.robot[0]); // Visualizo apenas um robô
        viewPheromoneMap(rows, columns, &grade);
        robotMove(rows, columns, &grade);
        for (int j = 0; j < grade.robotCount; j++)
        {
            pheromone_evaporate(&grade.robot[j], &grade, rows, columns, 0.005);
        }
    }

    return 0;
}
