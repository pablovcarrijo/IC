#include "Project.h"

void view_pheromone_robot_map(int rows, int columns, Robot *robot, Pool *pool);
void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta);
void view_real_robot_map(int rows, int columns, Robot *robot, Pool *pool);
void view_all_robot_real_map(int rows, int columns, int quantityRobots, Robot *robots, Pool *pool);
