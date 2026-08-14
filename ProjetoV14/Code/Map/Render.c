#include "Render.h"
#include "../Config/Project.h"
#include "raylib.h"

int choose_cell_size(int rows, int cols)
{

    int cellSize = 14;    

    return cellSize;
}

void draw_map(int **map, int rows, int cols, int cellSize)
{
    int padding = cellSize >= 16 ? 2 : 1;

    Color freeColor = (Color){38, 42, 52, 255};
    Color wallColor = (Color){210, 80, 80, 255};
    Color mapBackground = (Color){25, 28, 36, 255};

    Rectangle mapArea = {
        0,
        0,
        cols * cellSize,
        rows * cellSize};

    DrawRectangleRec(mapArea, mapBackground);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rectangle cell = {
                j * cellSize + padding,
                i * cellSize + padding,
                cellSize - padding * 2,
                cellSize - padding * 2};

            if (map[i][j] == CELL_WALL)
            {
                DrawRectangleRounded(cell, 0.25f, 8, wallColor);
            }
            else
            {
                DrawRectangleRounded(cell, 0.25f, 8, freeColor);
            }
        }
    }
}

void draw_panel(int screenWidth, int panelY, int maxCycles, int quantityRobots, int rows, int cols)
{
    Color panelColor = (Color){20, 22, 28, 255};
    Color lineColor = (Color){80, 85, 100, 255};

    DrawRectangle(0, panelY, screenWidth, PANEL_HEIGHT, panelColor);
    DrawLine(0, panelY, screenWidth, panelY, lineColor);

    DrawText(
        TextFormat("Ciclos lidos: %d", maxCycles),
        20,
        panelY + 12,
        20,
        WHITE);

    DrawText(
        TextFormat("Robos lidos: %d | Mapa: %d x %d", quantityRobots, rows, cols),
        20,
        panelY + 40,
        18,
        (Color){220, 220, 220, 255});

    DrawText(
        "Vermelho = parede | Cinza = caminho livre",
        360,
        panelY + 25,
        18,
        (Color){220, 220, 220, 255});
}

void draw_robots(Robot *robots, int quantityRobots, int cellSize, int selectedRobot)
{
    for (int i = 0; i < quantityRobots; i++)
    {
        int centerX = robots[i].pos.y * cellSize + cellSize / 2;
        int centerY = robots[i].pos.x * cellSize + cellSize / 2;

        Color robotColor = BLUE;

        if (i == selectedRobot)
        {
            robotColor = YELLOW;
        }

        DrawCircle(centerX, centerY, cellSize / 2.5f, robotColor);

        DrawText(
            TextFormat("%d", robots[i].id),
            centerX - 4,
            centerY - 6,
            12,
            BLACK
        );
    }
}

void draw_pheromone_map(int **map, Robot *robot, int rows, int cols, int cellSize)
{
    int padding = cellSize >= 16 ? 2 : 1;

    Color wallColor = (Color){210, 80, 80, 255};
    Color mapBackground = (Color){25, 28, 36, 255};

    Rectangle mapArea = {
        0,
        0,
        cols * cellSize,
        rows * cellSize
    };

    DrawRectangleRec(mapArea, mapBackground);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rectangle cell = {
                j * cellSize + padding,
                i * cellSize + padding,
                cellSize - padding * 2,
                cellSize - padding * 2
            };

            if (map[i][j] == CELL_WALL)
            {
                DrawRectangleRounded(cell, 0.25f, 8, wallColor);
            }
            else
            {
                double pheromone = robot->pheromoneGrade[i][j].pheromoneValue;

                if (pheromone < 0.0)
                {
                    pheromone = 0.0;
                }

                if (pheromone > 1.0)
                {
                    pheromone = 1.0;
                }

                int intensity = (int)(pheromone * 255);

                Color pheromoneColor = (Color){
                    20,
                    intensity,
                    20,
                    255
                };

                DrawRectangleRounded(cell, 0.25f, 8, pheromoneColor);
            }
        }
    }
}