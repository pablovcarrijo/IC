#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Project.h"
#include "loadMap.h"
#include "Robot.h"
#include "MapView.h"
#include "Communication.h"

int main()
{
    srand(time(NULL));
    int rows, columns, cyclos, quantityRobots, k = 1;
    RealMap realMap;
    Robot *robots;
    Pool *pool;

    get_map_cyclos_robots("../entradas.txt", &cyclos, &quantityRobots);

    get_map_size("../entradas.txt", &rows, &columns);


    robots = (Robot *)malloc(quantityRobots * sizeof(Robot));
    pool = (Pool *)malloc(sizeof(Pool));

    com_poolInit(pool, quantityRobots);

    realMap.value = malloc(rows * sizeof(int *));
    for(int i = 0; i < rows; i++){
        realMap.value[i] = malloc(columns * sizeof(int));
    }

    load_map_from_txt("../entradas.txt", rows, columns, &realMap);

    for(int i = 0; i < quantityRobots; i++){
        init_robots(k++, rows, columns, &robots[i]);

        com_setDetectionRadius(&robots[i], DETECTION_COMUNICATION_RADIUS);

        place_robot_random(rows, columns, i, &robots[i], &realMap);
    }

    for (int i = 0; i < cyclos; i++)
    {
        //view_pheromone_robot_map(rows, columns, &robots[0], pool);
        //view_pheromone_robot_map(rows, columns, &robots[1], pool);
        //view_pheromone_robot_map(rows, columns, &robots[2], pool);
        
        view_real_robot_map(rows, columns, &robots[0], pool);
        view_real_robot_map(rows, columns, &robots[1], pool);
        view_real_robot_map(rows, columns, &robots[2], pool);

        for(int i = 1; i <= quantityRobots; i++){
            robot_move(rows, columns, &robots[i-1], pool, i, &realMap);
            com_WriteOnPool(&robots[i-1], pool, rows, columns);
            com_ReadPool(&robots[i-1], pool);
        }
        
        for (int j = 0; j < quantityRobots; j++)
        {
            pheromone_robot_evaporate(&robots[j], rows, columns, 0.005);
        }
        
    }

    return 0;
}
