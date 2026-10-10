#ifndef TESTS_H
#define TESTS_H

#include "../Config/Project.h"
#include <stdio.h>



// Uma amostra da porcentagem coletiva em um ciclo.
typedef struct
{
    int cycle;
    double percentage;
} CollectiveSample;

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
// Pass NULL to measure coverage without changing the map displayed with Y.
// Reads the robots' maps without changing their knowledge or communication.
RobotWallStats tests_collective_wall_stats(const RealMap *realMap, const Robot *robots,
                                         int quantityRobots, int rows, int cols,
                                         int totalWalls, int **collectiveMap);

// Grava uma linha com as amostras e o primeiro ciclo de conclusao.
// Retorna 1 se gravou corretamente e 0 em caso de erro.
int tests_write_result(FILE *file, int experiment, unsigned int seed,
                       const CollectiveSample *samples, int sampleCount,
                       int firstCompleteCycle, int totalWalls);





// Number of simulation cycles between collective wall coverage measurements.
#define COLLECTIVE_MAPPING_INTERVAL 50
#define EXPERIMENT_COUNT 3
#define EXPERIMENT_INPUT_FILE "Config/entradas.txt"
#define EXPERIMENT_RESULTS_FILE "resultados_experimentos.txt"



#endif