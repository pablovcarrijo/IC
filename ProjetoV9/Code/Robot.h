#include "Project.h"

int init_robots(int id, int rows, int columns, Robot *robot);
void place_robot_random(int rows, int columns, Robot *robot);
void robot_move(int rows, int columns, Robot *robot, Pool *pool, int id);
int free_way(Robot *robot, int current_i, int current_j, int new_i, int new_j);
