#include <stdio.h>
#include <stdlib.h>
#include "Project.h"

int main()
{
    int rows, columns, cyclos, quantityRobots, k = 1;

    get_map_cyclos_robots("entradas.txt", &cyclos, &quantityRobots);

    get_map_size("entradas.txt", &rows, &columns);

    Robot *robots = (Robot *)malloc(quantityRobots * sizeof(Robot));
    for(int i = 0; i < quantityRobots; i++){
        init_robots((k++), rows, columns, &robots[i]);
        load_map_from_txt("entradas.txt", rows, columns, &robots[i]);
        place_robot_random(rows, columns, &robots[i]);
    }

    for (int i = 0; i < cyclos; i++)
    {
        view_pheromone_robot_map(rows, columns, &robots[0]);
        
        for(int i = 0; i < quantityRobots; i++){
            robot_move(rows, columns, &robots[i]);
        }
        
        for (int j = 0; j < quantityRobots; j++)
        {
            pheromone_robot_evaporate(&robots[j], rows, columns, 0.005);
        }
        
    }

    return 0;
}
