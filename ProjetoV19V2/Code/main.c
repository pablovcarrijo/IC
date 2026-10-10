#include "raylib.h"

#include "Config/Project.h"
#include "Map/LoadMap.h"
#include "Map/Render.h"
#include "Robot/Robot.h"
#include "Communication/Communication.h"
#include "Tests/Tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    const char *filePath = EXPERIMENT_INPUT_FILE;
    if (argc >= 2)
    {
        filePath = argv[1];
    }

    // Carrega o mapa que sera usado nos experimentos.
    int maxCycles = 0;
    int quantityRobots = 0;
    int rows = 0;
    int cols = 0;
    int **map = NULL;

    if (!load_map_from_txt(filePath, &maxCycles, &quantityRobots,
                           &rows, &cols, &map))
    {
        return 1;
    }

    int availableCells = 0;
    for (int row = 1; row < rows - 1; row++)
    {
        for (int col = 1; col < cols - 1; col++)
        {
            if (map[row][col] != CELL_WALL)
            {
                availableCells++;
            }
        }
    }
    if (maxCycles < 0 || quantityRobots < 0 || quantityRobots > availableCells)
    {
        printf("Erro: ciclos/robos invalidos ou sem espaco para posicionar os robos.\n");
        free_matrix(map, rows);
        return 1;
    }

    // Guarda as amostras de um experimento. O vetor sera reutilizado no proximo.
    int sampleCapacity = maxCycles / COLLECTIVE_MAPPING_INTERVAL + 2;
    CollectiveSample *samples = calloc(sampleCapacity, sizeof(CollectiveSample));
    int **collectiveMap = allocate_matrix(rows, cols, CELL_UNKNOWN);
    if (samples == NULL || collectiveMap == NULL)
    {
        printf("Erro: memoria insuficiente para os experimentos.\n");
        free(samples);
        free_matrix(collectiveMap, rows);
        free_matrix(map, rows);
        return 1;
    }

    FILE *resultsFile = fopen(EXPERIMENT_RESULTS_FILE, "a");
    if (resultsFile == NULL)
    {
        printf("Erro: nao foi possivel abrir %s.\n", EXPERIMENT_RESULTS_FILE);
        free(samples);
        free_matrix(collectiveMap, rows);
        free_matrix(map, rows);
        return 1;
    }

    RealMap realMap;
    realMap.value = map;
    int totalWalls = tests_count_total_walls(&realMap, rows, cols);

    // Abre uma janela para acompanhar todos os experimentos.
    float cellWidth = 900.0f / cols;
    float cellHeight = 600.0f / rows;
    float cellSize = cellWidth;
    if (cellHeight < cellSize)
    {
        cellSize = cellHeight;
    }
    int screenWidth = (int)(cols * cellSize);
    if (screenWidth < 720)
    {
        screenWidth = 720;
    }
    int panelY = (int)(rows * cellSize);
    InitWindow(screenWidth, panelY + PANEL_HEIGHT, "Experimentos de mapeamento");
    SetTargetFPS(60);

    int closeRequested = 0;
    int error = 0;

    for (int experiment = 1; experiment <= EXPERIMENT_COUNT; experiment++)
    {
        // Cada repeticao comeca com novos robos, mapas, feromonios e comunicacao.
        srand(experiment);
        SetWindowTitle(TextFormat("Experimento %d de %d", experiment, EXPERIMENT_COUNT));

        Pool pool;
        com_poolInit(&pool, quantityRobots);
        Robot *robots = NULL;
        RobotWallStats *wallStats = NULL;
        if (quantityRobots > 0)
        {
            robots = calloc(quantityRobots, sizeof(Robot));
            wallStats = calloc(quantityRobots, sizeof(RobotWallStats));
            if (robots == NULL || wallStats == NULL)
            {
                free(robots);
                free(wallStats);
                error = 1;
                break;
            }
        }

        int initializedRobots = 0;
        for (int i = 0; i < quantityRobots; i++)
        {
            if (!init_robots(i + 1, rows, cols, &robots[i]))
            {
                error = 1;
                break;
            }
            initializedRobots++;
            com_setDetectionRadius(&robots[i], DETECTION_COMUNICATION_RADIUS);
            place_robot_random(rows, cols, i, robots, &robots[i], &realMap);
        }
        if (error)
        {
            for (int i = 0; i < initializedRobots; i++)
            {
                free_robot(&robots[i], rows);
            }
            free(robots);
            free(wallStats);
            break;
        }

        int cycle = 0;
        int frameCounter = 0;
        int viewMode = 0;
        int showCollective = 0;
        int isPaused = 0;
        int selectedRobot = 0;
        int collectiveCycle = 0;
        int resultSaved = 0;
        int sampleCount = 0;
        int firstCompleteCycle = -1;

        double averagePercentage = tests_update_wall_stats(&realMap, robots,
                                    quantityRobots, rows, cols, totalWalls, wallStats);
        RobotWallStats collectiveStats = tests_collective_wall_stats(&realMap, robots,
                                          quantityRobots, rows, cols, totalWalls, collectiveMap);
        samples[sampleCount++] = (CollectiveSample){0, collectiveStats.percentage};
        if (totalWalls > 0 && collectiveStats.mappedWalls == totalWalls)
        {
            firstCompleteCycle = 0;
        }

        while (!closeRequested)
        {
            if (WindowShouldClose())
            {
                closeRequested = 1;
                break;
            }

            if (IsKeyPressed(KEY_A) && cycle < maxCycles)
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
            if (quantityRobots > 0)
            {
                if (IsKeyPressed(KEY_RIGHT))
                {
                    selectedRobot = (selectedRobot + 1) % quantityRobots;
                }
                if (IsKeyPressed(KEY_LEFT))
                {
                    selectedRobot--;
                    if (selectedRobot < 0)
                    {
                        selectedRobot = quantityRobots - 1;
                    }
                }
            }

            if (!isPaused && cycle < maxCycles && frameCounter == 0)
            {
                // Movimenta todos os robos.
                for (int i = 0; i < quantityRobots; i++)
                {
                    robot_move(rows, cols, i + 1, quantityRobots, &robots[i], robots, &pool, &realMap);
                }

                // Atualiza as mensagens que cada robo compartilha.
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

                // Recebe as mensagens e evapora o feromonio.
                for (int i = 0; i < quantityRobots; i++)
                {
                    com_ReadPool(&robots[i], &pool);
                }
                for (int i = 0; i < quantityRobots; i++)
                {
                    pheromone_robot_evaporate(&robots[i], rows, cols, 0.005);
                }
                cycle++;

                averagePercentage = tests_update_wall_stats(&realMap, robots,
                                    quantityRobots, rows, cols, totalWalls, wallStats);

                // Verifica a conclusao em TODO ciclo, sem alterar a copia exibida com Y.
                RobotWallStats currentStats = tests_collective_wall_stats(&realMap, robots,
                                               quantityRobots, rows, cols, totalWalls, NULL);
                if (firstCompleteCycle == -1 && totalWalls > 0 &&
                    currentStats.mappedWalls == totalWalls)
                {
                    firstCompleteCycle = cycle;
                }

                // Salva a porcentagem e a copia do mapa a cada intervalo e no final.
                if (cycle % COLLECTIVE_MAPPING_INTERVAL == 0 || cycle == maxCycles)
                {
                    collectiveStats = tests_collective_wall_stats(&realMap, robots,
                                      quantityRobots, rows, cols, totalWalls, collectiveMap);
                    collectiveCycle = cycle;
                    samples[sampleCount++] = (CollectiveSample){cycle, collectiveStats.percentage};
                }
            }

            // Escreve uma unica linha ao terminar este experimento.
            if (cycle == maxCycles && !resultSaved)
            {
                if (!tests_write_result(resultsFile, experiment, experiment, samples,
                                         sampleCount, firstCompleteCycle, totalWalls))
                {
                    error = 1;
                    break;
                }
                resultSaved = 1;
                printf("Experimento %d de %d salvo.\n", experiment, EXPERIMENT_COUNT);
                if (experiment == EXPERIMENT_COUNT)
                {
                    SetWindowTitle("Experimentos finalizados - resultados salvos");
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
                draw_collective_map(collectiveMap, rows, cols, cellSize, cellSize);
            }
            else if (viewMode == 0 || quantityRobots == 0)
            {
                draw_map(map, rows, cols, cellSize, cellSize);
            }
            else if (viewMode == 1)
            {
                draw_pheromone_map(map, &robots[selectedRobot], rows, cols, cellSize, cellSize);
            }
            else
            {
                draw_robot_memory_map(&robots[selectedRobot], rows, cols, cellSize, cellSize);
            }

            if (!showCollective)
            {
                if (viewMode == 2 && quantityRobots > 0)
                {
                    draw_robots(&robots[selectedRobot], 1, cellSize, cellSize, 0);
                }
                else
                {
                    draw_robots(robots, quantityRobots, cellSize, cellSize, selectedRobot);
                }
            }

            int selectedRobotId = 0;
            RobotWallStats *selectedStats = NULL;
            if (quantityRobots > 0)
            {
                selectedRobotId = robots[selectedRobot].id;
                selectedStats = &wallStats[selectedRobot];
            }
            draw_panel(screenWidth, panelY, cycle, maxCycles, quantityRobots, rows, cols,
                       selectedRobotId, totalWalls, selectedStats, averagePercentage,
                       &collectiveStats, collectiveCycle, showCollective, isPaused);
            EndDrawing();

            // Avanca automaticamente. Depois do ultimo, deixa a janela aberta.
            if (resultSaved && experiment < EXPERIMENT_COUNT)
            {
                break;
            }
        }

        // Libera as mensagens e os robos antes da proxima repeticao.
        Message *message = pool.msgs;
        while (message != NULL)
        {
            Message *next = message->prox;
            free(message->pathMemory.cells);
            free(message);
            message = next;
        }
        for (int i = 0; i < quantityRobots; i++)
        {
            free_robot(&robots[i], rows);
        }
        free(robots);
        free(wallStats);

        if (closeRequested || error)
        {
            break;
        }
    }

    CloseWindow();
    if (fclose(resultsFile) != 0)
    {
        error = 1;
    }
    free(samples);
    free_matrix(collectiveMap, rows);
    free_matrix(map, rows);
    if (error)
    {
        printf("Erro ao executar ou salvar os experimentos.\n");
    }
    return error;
}
