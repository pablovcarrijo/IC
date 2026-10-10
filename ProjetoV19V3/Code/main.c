#include "raylib.h"

#include "Config/Project.h"
#include "Map/LoadMap.h"
#include "Map/Render.h"
#include "Robot/Robot.h"
#include "Communication/Communication.h"
#include "Tests/Tests.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));

    const char *filePath = "Config/entradas.txt";

    if (argc >= 2)
    {
        filePath = argv[1];
    }

    int maxCycles = 0;
    int quantityRobots = 0;
    int rows = 0;
    int cols = 0;
    int **map = NULL;

    if (!load_map_from_txt(filePath, &maxCycles, &quantityRobots, &rows, &cols, &map))
    {
        return 1;
    }

    Robot *robots = quantityRobots > 0 ? malloc(quantityRobots * sizeof(Robot)) : NULL;
    RobotWallStats *wallStats = quantityRobots > 0
                                   ? malloc(quantityRobots * sizeof(RobotWallStats)) : NULL;

    if (quantityRobots > 0 && (robots == NULL || wallStats == NULL))
    {
        free(robots);
        free(wallStats);
        free_matrix(map, rows);
        return 1;
    }

    Pool pool;

    if (!com_poolInit(&pool, quantityRobots))
    {
        free_matrix(map, rows);
        free(robots);
        free(wallStats);
        return 1;
    }

    RealMap realMap;
    realMap.value = map;

    for (int i = 0; i < quantityRobots; i++)
    {
        if (!init_robots(i + 1, rows, cols, &robots[i]))
        {
            free_matrix(map, rows);
            free(robots);
            free(wallStats);
            return 1;
        }

        com_setDetectionRadius(&robots[i], DETECTION_COMUNICATION_RADIUS);

        place_robot_random(rows, cols, i, robots, &robots[i], &realMap);
    }

    int **collectiveMap = allocate_matrix(rows, cols, CELL_UNKNOWN);
    if (collectiveMap == NULL)
    {
        for (int i = 0; i < quantityRobots; i++)
        {
            free_robot(&robots[i], rows);
        }
        free(robots);
        free(wallStats);
        free_matrix(map, rows);
        return 1;
    }

    int totalWalls = tests_count_total_walls(&realMap, rows, cols);
    double averagePercentage = tests_update_wall_stats(&realMap, robots, quantityRobots,
                                                       rows, cols, totalWalls, wallStats);
    RobotWallStats collectiveStats = tests_collective_wall_stats(&realMap, robots,
                                                                quantityRobots, rows, cols,
                                                                totalWalls, collectiveMap);
    int collectiveCycle = 0;

    const int mapViewWidth = 900;
    const int mapViewHeight = 600;
    float cellWidthLimit = (float)mapViewWidth / cols;
    float cellHeightLimit = (float)mapViewHeight / rows;
    float cellSize = cellWidthLimit < cellHeightLimit ? cellWidthLimit : cellHeightLimit;
    float cellWidth = cellSize;
    float cellHeight = cellSize;
    int screenWidth = (int)(cols * cellWidth);
    if (screenWidth < 720)
    {
        screenWidth = 720;
    }
    int panelY = (int)(rows * cellHeight);
    int screenHeight = panelY + PANEL_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Simulador de Robos - Raylib");
    SetTargetFPS(60);

    int cycle = 0;
    int frameCounter = 0;
    int viewMode = 0;
    int showCollective = 0;
    int isPaused = 0;
    int selectedRobot = 0;

    while (!WindowShouldClose())
    {
        if (cycle < maxCycles && IsKeyPressed(KEY_A))
        {
            isPaused = !isPaused;
        }

        if (IsKeyPressed(KEY_Y))
        {
            showCollective = !showCollective;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            showCollective = 0;
            viewMode = (viewMode + 1) % 3;
        }

        if (quantityRobots > 0 && IsKeyPressed(KEY_RIGHT))
        {
            selectedRobot++;

            if (selectedRobot >= quantityRobots)
            {
                selectedRobot = 0;
            }
        }

        if (quantityRobots > 0 && IsKeyPressed(KEY_LEFT))
        {
            selectedRobot--;

            if (selectedRobot < 0)
            {
                selectedRobot = quantityRobots - 1;
            }
        }
        if (!isPaused && cycle < maxCycles && frameCounter == 0)
        {
            for (int i = 0; i < quantityRobots; i++)
            {
                robot_move(rows, cols, i + 1, quantityRobots, &robots[i], robots, &pool, &realMap);
            }

            for (int i = 0; i < quantityRobots; i++)
            {
                if (robots[i].stepsSinceLastQueueWrite >= QUEUE_WRITE_STEP_INTERVAL)
                {
                    if (com_WriteOnPool(&robots[i], &pool, rows, cols))
                    {
                        robots[i].stepsSinceLastQueueWrite = 0;
                    }
                }
            }

            for (int i = 0; i < quantityRobots; i++)
            {
                com_ReadPool(&robots[i], &pool);
            }

            for (int i = 0; i < quantityRobots; i++)
            {
                pheromone_robot_evaporate(&robots[i], rows, cols, 0.005);
            }

            cycle++;
            averagePercentage = tests_update_wall_stats(&realMap, robots, quantityRobots,
                                                        rows, cols, totalWalls, wallStats);
            if (cycle % COLLECTIVE_MAPPING_INTERVAL == 0 || cycle == maxCycles)
            {
                collectiveStats = tests_collective_wall_stats(&realMap, robots,
                                                              quantityRobots, rows, cols,
                                                              totalWalls, collectiveMap);
                collectiveCycle = cycle;
            }
        }

        if (!isPaused)
        {
            update_robot_animation(robots, quantityRobots, 0.18f);
            frameCounter = (frameCounter + 1) % 12;
        }

        BeginDrawing();

        ClearBackground((Color){18, 20, 26, 255});

        if (showCollective)
        {
            draw_collective_map(collectiveMap, rows, cols, cellWidth, cellHeight);
        }
        else if (viewMode == 0 || quantityRobots == 0)
        {
            draw_map(map, rows, cols, cellWidth, cellHeight);
        }
        else if (viewMode == 1)
        {
            draw_pheromone_map(map, &robots[selectedRobot], rows, cols, cellWidth, cellHeight);
        }
        else
        {
            draw_robot_memory_map(&robots[selectedRobot], rows, cols, cellWidth, cellHeight);
        }

        if (!showCollective && viewMode == 2 && quantityRobots > 0)
        {
            draw_robots(&robots[selectedRobot], 1, cellWidth, cellHeight, 0);
        }
        else if (!showCollective)
        {
            draw_robots(robots, quantityRobots, cellWidth, cellHeight, selectedRobot);
        }

        draw_panel(screenWidth, panelY, cycle, maxCycles, quantityRobots, rows, cols,
                   quantityRobots > 0 ? robots[selectedRobot].id : 0,
                   totalWalls, quantityRobots > 0 ? &wallStats[selectedRobot] : NULL,
                   averagePercentage, &collectiveStats, collectiveCycle, showCollective, isPaused);

        EndDrawing();
    }

    CloseWindow();

    for (int i = 0; i < quantityRobots; i++)
    {
        free_robot(&robots[i], rows);
    }

    free(robots);
    free(wallStats);
    free_matrix(collectiveMap, rows);
    free_matrix(map, rows);

    return 0;
}
