#include <stdio.h>
#include <stdlib.h>
#include "Project.h"

int main()
{

    Map grade;
    int rows, columns, cyclos, quantityRobots;

    get_map_cyclos_robots("entradas.txt", &cyclos, &quantityRobots);

    get_map_size("entradas.txt", &rows, &columns);

    init_map(rows, columns, &grade);

    load_map_from_txt("entradas.txt", rows, columns, &grade);

    robots_map(quantityRobots, rows, columns, &grade);

    for (int i = 0; i < cyclos; i++)
    {
        // view_map(rows, columns, &grade); // Visualizo o mapa todo com todos os robôs
        // view_robot_map(rows, columns, &grade.robot[0]); // Visualizo apenas um robô
        view_pheromone_map(rows, columns, &grade);
        robot_move(rows, columns, &grade);
        for (int j = 0; j < grade.robotCount; j++)
        {
            pheromone_robot_evaporate(&grade.robot[j], rows, columns, 0.005);
        }
        global_pheromone_evaporate(&grade, rows, columns, 0.005);
    }

    return 0;
}
