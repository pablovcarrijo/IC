#ifndef ROBOT_H
#define ROBOT_H

#include "../Config/Project.h"

int init_robots(int id, int rows, int columns, Robot *robot);

void place_robot_random(int rows, int columns, int quantityPlaced, Robot *robots, Robot *robot, RealMap *realMap);

void robot_move(int rows, int columns, Robot *robot, Pool *pool, int id, RealMap *realMap);

void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta);

void free_robot(Robot *robot, int rows);

int get_step_direction(int target, int current);

int verify_position_occupied(Robot *robots, int quantityPlaced, int x, int y);

int free_way(Robot *robot, int current_i, int current_j, int new_i, int new_j);

#endif