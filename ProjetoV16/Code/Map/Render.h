#ifndef RENDER_H
#define RENDER_H

#include "../Config/Project.h"

int choose_cell_size(int rows, int cols);

void draw_map(int **map, int rows, int cols, float cellWidth, float cellHeight);

void draw_robots(Robot *robots, int quantityRobots, float cellWidth, float cellHeight, int selectedRobot);

void draw_panel(int screenWidth, int panelY, int maxCycles, int quantityRobots, int rows, int cols);

void draw_pheromone_map(int **map, Robot *robot, int rows, int cols, float cellWidth, float cellHeight);

void update_robot_animation(Robot *robots, int quantityRobots, float speed);

#endif
