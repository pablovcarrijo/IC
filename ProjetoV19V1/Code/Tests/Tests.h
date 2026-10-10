#ifndef TESTS_H
#define TESTS_H

#include "../Config/Project.h"

// Number of simulation cycles between collective wall coverage measurements.
#define COLLECTIVE_MAPPING_INTERVAL 50

#if COLLECTIVE_MAPPING_INTERVAL <= 0
#error "COLLECTIVE_MAPPING_INTERVAL must be greater than zero"
#endif

typedef struct
{
    int mappedWalls;
    double percentage;
} RobotWallStats;

int tests_count_total_walls(const RealMap *realMap, int rows, int cols);

// Updates each robot's coverage and returns the arithmetic mean.
// With no walls or no robots, returns 0; the UI displays an explanatory label.
double tests_update_wall_stats(const RealMap *realMap, const Robot *robots,
                              int quantityRobots, int rows, int cols,
                              int totalWalls, RobotWallStats *stats);

// Saves the union as CELL_WALL/CELL_UNKNOWN and returns matching coverage.
// The caller owns collectiveMap; it stays unchanged until the next measurement.
// Reads the robots' maps without changing their knowledge or communication.
RobotWallStats tests_collective_wall_stats(const RealMap *realMap, const Robot *robots,
                                         int quantityRobots, int rows, int cols,
                                         int totalWalls, int **collectiveMap);

#endif
