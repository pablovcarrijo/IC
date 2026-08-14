#include "Render.h"
#include "../Config/Project.h"
#include "raylib.h"

int choose_cell_size(int rows, int cols)
{

    int cellSize = 12;

    return cellSize;
}

void draw_map(int **map, int rows, int cols, float cellWidth, float cellHeight)
{
    float padding = 1.0f;

    Color freeColor = (Color){38, 42, 52, 255};
    Color wallColor = (Color){210, 80, 80, 255};
    Color mapBackground = (Color){25, 28, 36, 255};

    Rectangle mapArea = {
        0,
        0,
        cols * cellWidth,
        rows * cellHeight};

    DrawRectangleRec(mapArea, mapBackground);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rectangle cell = {
                j * cellWidth + padding,
                i * cellHeight + padding,
                cellWidth - padding * 2,
                cellHeight - padding * 2};

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

    DrawText(TextFormat("Ciclos: %d - Robos: %d - Mapa: %d x %d", maxCycles, quantityRobots, rows, cols),
                        20, panelY + 18, 20, WHITE);
}

void draw_robots(Robot *robots, int quantityRobots, float cellWidth, float cellHeight, int selectedRobot)
{
    float robotSize = cellWidth < cellHeight ? cellWidth : cellHeight;

    for (int i = 0; i < quantityRobots; i++)
    {
        int centerX = (int)(robots[i].drawCol * cellWidth + cellWidth / 2);
        int centerY = (int)(robots[i].drawRow * cellHeight + cellHeight / 2);

        Color robotColor = BLUE;

        if (i == selectedRobot)
        {
            robotColor = YELLOW;
        }

        DrawCircle(centerX, centerY, robotSize / 2.5f, robotColor);

        DrawText(
            TextFormat("%d", robots[i].id),
            centerX - 4,
            centerY - 6,
            12,
            BLACK);
    }
}

void draw_pheromone_map(int **map, Robot *robot, int rows, int cols, float cellWidth, float cellHeight)
{
    float padding = 1.0f;

    Color wallColor = (Color){210, 80, 80, 255};
    Color mapBackground = (Color){25, 28, 36, 255};

    Rectangle mapArea = {
        0,
        0,
        cols * cellWidth,
        rows * cellHeight};

    DrawRectangleRec(mapArea, mapBackground);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rectangle cell = {
                j * cellWidth + padding,
                i * cellHeight + padding,
                cellWidth - padding * 2,
                cellHeight - padding * 2};

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
                    255};

                DrawRectangleRounded(cell, 0.25f, 8, pheromoneColor);
            }
        }
    }
}

void draw_robot_memory_map(Robot *robot, int rows, int cols, float cellWidth, float cellHeight)
{
    float padding = 1.0f;
    Color unknownColor = (Color){12, 14, 18, 255};
    Color freeColor = (Color){38, 42, 52, 255};
    Color visitedColor = (Color){65, 130, 150, 255};
    Color wallColor = (Color){210, 80, 80, 255};
    Color robotColor = (Color){235, 195, 65, 255};

    DrawRectangle(0, 0, (int)(cols * cellWidth), (int)(rows * cellHeight), unknownColor);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rectangle cell = {
                j * cellWidth + padding,
                i * cellHeight + padding,
                cellWidth - padding * 2,
                cellHeight - padding * 2};
            Color cellColor = unknownColor;

            if (robot->mapRobot[i][j] == CELL_WALL) cellColor = wallColor;
            else if (robot->mapRobot[i][j] == CELL_VISITED) cellColor = visitedColor;
            else if (robot->mapRobot[i][j] == CELL_FREE) cellColor = freeColor;
            else if (robot->mapRobot[i][j] == CELL_ROBOT) cellColor = robotColor;

            DrawRectangleRounded(cell, 0.25f, 8, cellColor);
        }
    }
}

void update_robot_animation(Robot *robots, int quantityRobots, float speed)
{
    for (int i = 0; i < quantityRobots; i++)
    {
        robots[i].drawRow += ((float)robots[i].pos.x - robots[i].drawRow) * speed;
        robots[i].drawCol += ((float)robots[i].pos.y - robots[i].drawCol) * speed;
    }
}
