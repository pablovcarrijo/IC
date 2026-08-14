#include "raylib.h"

#include "Config/Project.h"
#include "Map/LoadMap.h"
#include "Map/Render.h"
#include "Robot/Robot.h"
#include "Communication/Communication.h"

#include <stdlib.h>
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

    Robot *robots = malloc(quantityRobots * sizeof(Robot));

    if (robots == NULL)
    {
        free_matrix(map, rows);
        return 1;
    }

    Pool pool;

    if (!com_poolInit(&pool, quantityRobots))
    {
        free_matrix(map, rows);
        free(robots);
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
            return 1;
        }

        com_setDetectionRadius(&robots[i], DETECTION_COMUNICATION_RADIUS);

        place_robot_random(rows, cols, i, robots, &robots[i], &realMap);
    }

    const int mapViewWidth = 900;
    const int mapViewHeight = 600;
    float cellWidthLimit = (float)mapViewWidth / cols;
    float cellHeightLimit = (float)mapViewHeight / rows;
    float cellSize = cellWidthLimit < cellHeightLimit ? cellWidthLimit : cellHeightLimit;
    float cellWidth = cellSize;
    float cellHeight = cellSize;
    int screenWidth = (int)(cols * cellWidth);
    int panelY = (int)(rows * cellHeight);
    int screenHeight = panelY + PANEL_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Simulador de Robos - Raylib");
    SetTargetFPS(60);

    int cycle = 0;
    int frameCounter = 0;
    int viewMode = 0;
    int selectedRobot = 0;

    while (!WindowShouldClose() && cycle < maxCycles)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            viewMode = (viewMode + 1) % 3;
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            selectedRobot++;

            if (selectedRobot >= quantityRobots)
            {
                selectedRobot = 0;
            }
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            selectedRobot--;

            if (selectedRobot < 0)
            {
                selectedRobot = quantityRobots - 1;
            }
        }
        if (frameCounter % 12 == 0)
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
        }

        update_robot_animation(robots, quantityRobots, 0.18f);

        frameCounter++;

        BeginDrawing();

        ClearBackground((Color){18, 20, 26, 255});

        if (viewMode == 0)
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

        if (viewMode == 2)
        {
            draw_robots(&robots[selectedRobot], 1, cellWidth, cellHeight, 0);
        }
        else
        {
            draw_robots(robots, quantityRobots, cellWidth, cellHeight, selectedRobot);
        }

        draw_panel(screenWidth, panelY, maxCycles, quantityRobots, rows, cols);

        EndDrawing();
    }

    CloseWindow();

    for (int i = 0; i < quantityRobots; i++)
    {
        free_robot(&robots[i], rows);
    }

    free(robots);
    free_matrix(map, rows);

    return 0;
}
