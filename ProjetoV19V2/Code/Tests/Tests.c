#include "Tests.h"

int tests_count_total_walls(const RealMap *realMap, int rows, int cols)
{
    int total = 0;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (realMap->value[row][col] == CELL_WALL)
            {
                total++;
            }
        }
    }

    return total;
}

double tests_update_wall_stats(const RealMap *realMap, const Robot *robots,
                              int quantityRobots, int rows, int cols,
                              int totalWalls, RobotWallStats *stats)
{
    double sum = 0.0;

    for (int i = 0; i < quantityRobots; i++)
    {
        stats[i].mappedWalls = 0;

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                // Count each real wall once, including walls learned from peers.
                if (realMap->value[row][col] == CELL_WALL &&
                    robots[i].mapRobot[row][col] == CELL_WALL)
                {
                    stats[i].mappedWalls++;
                }
            }
        }

        stats[i].percentage = totalWalls > 0
                                  ? 100.0 * stats[i].mappedWalls / totalWalls
                                  : 0.0;
        sum += stats[i].percentage;
    }

    return quantityRobots > 0 ? sum / quantityRobots : 0.0;
}

RobotWallStats tests_collective_wall_stats(const RealMap *realMap, const Robot *robots,
                                         int quantityRobots, int rows, int cols,
                                         int totalWalls, int **collectiveMap)
{
    RobotWallStats collective = {0};

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (collectiveMap != NULL)
            {
                collectiveMap[row][col] = CELL_UNKNOWN;
            }
            if (realMap->value[row][col] != CELL_WALL)
            {
                continue;
            }

            for (int i = 0; i < quantityRobots; i++)
            {
                if (robots[i].mapRobot[row][col] == CELL_WALL)
                {
                    if (collectiveMap != NULL)
                    {
                        collectiveMap[row][col] = CELL_WALL;
                    }
                    collective.mappedWalls++;
                    break;
                }
            }
        }
    }

    collective.percentage = totalWalls > 0
                                ? 100.0 * collective.mappedWalls / totalWalls
                                : 0.0;
    return collective;
}

int tests_write_result(FILE *file, int experiment, unsigned int seed,
                              const CollectiveSample *samples, int sampleCount,
                              int firstCompleteCycle, int totalWalls)
{
    if (fprintf(file, "experimento=%d;semente=%u", experiment, seed) < 0)
    {
        return 0;
    }

    for (int i = 0; i < sampleCount; i++)
    {
        int written;
        if (totalWalls > 0)
        {
            written = fprintf(file, ";ciclo_%d=%.6f%%", samples[i].cycle, samples[i].percentage);
        }
        else
        {
            written = fprintf(file, ";ciclo_%d=N/A", samples[i].cycle);
        }
        if (written < 0)
        {
            return 0;
        }
    }

    int written;
    if (totalWalls == 0)
    {
        written = fprintf(file, ";ciclo_100pct=sem_paredes\n");
    }
    else if (firstCompleteCycle < 0)
    {
        written = fprintf(file, ";ciclo_100pct=nao_atingiu\n");
    }
    else
    {
        written = fprintf(file, ";ciclo_100pct=%d\n", firstCompleteCycle);
    }

    if (written < 0 || fflush(file) != 0)
    {
        return 0;
    }
    return 1;
}
