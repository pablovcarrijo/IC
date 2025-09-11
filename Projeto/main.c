#include <stdio.h>
#include <stdlib.h>
#include "Project.h"

int main(){

    Map grade;
    Position pos;
    int rows, columns, ciclos, quantityRobots, percentage = 20;
    double totalSumDeposition;

    printf("Rows: ");
    scanf("%d%*c", &rows);
    printf("Columns: ");
    scanf("%d%*c", &columns);
    printf("Ciclos: ");
    scanf("%d%*c", &ciclos);
    printf("Quantidade de robos: ");
    scanf("%d%*c", &quantityRobots);

    init_map(rows, columns, &grade);
    robotsMap(quantityRobots, rows, columns, &grade);
    
    for(int i = 0; i < ciclos; i++){
        totalSumDeposition = 0;
        viewMap(rows, columns, grade);
        robotMove(rows, columns, &grade);
        pheromone_evaporate(rows, columns, &grade, percentage, 0.005, totalSumDeposition);
    }

    return 0;
}