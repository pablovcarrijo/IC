#ifndef RENDER_H
#define RENDER_H

#include "../Config/Project.h"

int choose_cell_size(int rows, int cols);

void draw_map(int **map, int rows, int cols, int cellSize);

void draw_robots(Robot *robots, int quantityRobots, int cellSize, int selectedRobot);

void draw_panel(int screenWidth, int panelY, int maxCycles, int quantityRobots, int rows, int cols);

void draw_pheromone_map(int **map, Robot *robot, int rows, int cols, int cellSize);

void update_robot_animation(Robot *robots, int quantityRobots, float speed);

#endif