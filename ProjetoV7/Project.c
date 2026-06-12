#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include "Project.h"

void init_robots(int id, int rows, int columns, Robot *robot, Pool *pool)
{
    robot->id = id;
    robot->mapRobot = (int **)malloc(rows * sizeof(int *));
    robot->pheromoneGrade = (Cell **)malloc(rows * sizeof(Cell *));

    robot->detectionRadius = DETECTION_PHEROMONE_RADIUS;

    init_queue(&pool->pathMemory, QUEUE_MAX);
    pool->idRobot = id;

    for (int i = 0; i < rows; i++)
    {
        robot->pheromoneGrade[i] = (Cell *)malloc(columns * sizeof(Cell));
        (*robot).mapRobot[i] = (int *)malloc(columns * sizeof(int));

        for (int j = 0; j < columns; j++)
        {
            robot->pheromoneGrade[i][j].pos.x = i;
            robot->pheromoneGrade[i][j].pos.y = j;
            robot->pheromoneGrade[i][j].pheromoneValue = 0;
        }
    }
}

void place_robot_random(int rows, int columns, Robot *robot)
{
    int x, y;
    srand(time(NULL));

    do
    {
        x = (rand() % (rows - 2)) + 1;
        y = (rand() % (columns - 2)) + 1;
    } while (robot->mapRobot[x][y] == CELL_WALL);

    robot->pos.x = x;
    robot->pos.y = y;
    robot->mapRobot[x][y] = CELL_ROBOT;
}

void robot_move(int rows, int columns, Robot *robot, Pool *pool, int id)
{

    double max_phero_cel = 1.0; // max of pheromone per cell
    double alpha = 0.5;         // maximum possible amount of deposited pheromone
    double eta = 2.0;           // compensation of the environment evaporation rate
    double euler = 2.718281;    // Euler
    double delta = 0.1;         // influence of the pheromone deposition rate
    double PI = 3.141592;       // PI value
    int r = 1;                  // Distance of the cell {i,j} from the robot {r E N | 0 <= r <= rd}

    int neighbors_i[MAX_NEIGHBORS], neighbors_j[MAX_NEIGHBORS], neighbor_i, neighbor_j, n = 0;
    double vectWeight[MAX_NEIGHBORS], sum = 0.0, contribution = 0.5, acumulated = 0.0, randomNumber;
    int new_i, new_j, i, j;

    Cell visitedCell;

    for (i = -DETECTION_PHEROMONE_RADIUS; i <= DETECTION_PHEROMONE_RADIUS; i++)
    {
        for (j = -DETECTION_PHEROMONE_RADIUS; j <= DETECTION_PHEROMONE_RADIUS; j++)
        {
            if (j == 0 && i == 0)
                continue;

            neighbor_i = robot->pos.x + i;
            neighbor_j = robot->pos.y + j;

            if (neighbor_i >= 0 && neighbor_j >= 0 && neighbor_i < rows && neighbor_j < columns)
            {
                if (robot->mapRobot[neighbor_i][neighbor_j] != CELL_WALL)
                {

                    double psi_t = robot->pheromoneGrade[neighbor_i][neighbor_j].pheromoneValue;

                    // Deposit pheromone on neighboring cells
                    double deltaPsi = (max_phero_cel - psi_t) * // Calc of new phremone around the robot
                                      (alpha * pow(delta * euler, (eta) * (r / PI)));

                    robot->pheromoneGrade[neighbor_i][neighbor_j].pheromoneValue += deltaPsi; // Update the pheromone value around the robot

                    neighbors_i[n] = neighbor_i;
                    neighbors_j[n] = neighbor_j;
                    vectWeight[n] = robot->pheromoneGrade[neighbor_i][neighbor_j].pheromoneValue;
                    n++;
                }
            }
        }
    }

    robot->pheromoneGrade[robot->pos.x][robot->pos.y].pheromoneValue +=
        ((1.0 - robot->pheromoneGrade[robot->pos.x][robot->pos.y].pheromoneValue) * (1.3 * contribution));

    for (i = 0; i < n; i++)
    {
        vectWeight[i] = 1.0 - vectWeight[i];
        sum += vectWeight[i];
    }

    for (i = 0; i < n; i++)
    {
        vectWeight[i] = vectWeight[i] / sum;
    }

    randomNumber = ((double)rand()) / RAND_MAX;
    for (i = 0; i < n; i++)
    {
        acumulated += vectWeight[i];
        if (acumulated >= randomNumber)
        {
            break;
        }
    }
    i = (i < n) ? i : i - 1;
    new_i = neighbors_i[i];
    new_j = neighbors_j[i];

    robot->mapRobot[robot->pos.x][robot->pos.y] = CELL_VISITED;
    robot->mapRobot[new_i][new_j] = CELL_ROBOT;

    robot->pos.x = pool->pos.x = visitedCell.pos.x = new_i;
    robot->pos.y = pool->pos.y = visitedCell.pos.y = new_j;

    visitedCell.pheromoneValue = robot->pheromoneGrade[new_i][new_j].pheromoneValue;

    enqueue_cell(&pool->pathMemory, visitedCell);
}

void robot_comunication(int rows, int columns, int quantityRobots, Robot *robot, Robot *robots, Pool *pool)
{

    for (int i = 0; i < quantityRobots; i++)
    {
        if (robots[i].id == robot->id)
            continue;

        int dx = robot->pos.x - robots[i].pos.x;
        int dy = robot->pos.y - robots[i].pos.y;

        double distance = sqrt((dx * dx) + (dy * dy));

        if (distance <= DETECTION_COMUNICATION_RADIUS)
        {
            printf("Robo %d encontrou o robo %d\n", robot->id, robots[i].id);
            Sleep(1000);
        }
    }
}

void view_pheromone_robot_map(int rows, int columns, Robot *robot, Pool *pool)
{
    system("cls");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (robot->mapRobot[i][j] == CELL_ROBOT)
            {
                printf("\033[48;2;0;0;255m  \033[m");
            }
            else if (robot->mapRobot[i][j] != CELL_WALL)
            {
                int intensity = (int)(robot->pheromoneGrade[i][j].pheromoneValue * 255);

                printf("\033[48;2;0;%d;0m  \033[m", intensity);
            }
            else
            {
                printf("\033[48;2;255;0;0m  \033[m");
            }
        }
        printf("\n");
    }
    printf("Posicao robo-> x: %d y: %d\n", pool->pos.x, pool->pos.y);
    print_queue(&pool->pathMemory);

    Sleep(700);
}

void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            robot->pheromoneGrade[i][j].pheromoneValue -= beta * robot->pheromoneGrade[i][j].pheromoneValue;
        }
    }
}

int load_map_from_txt(const char *filePath, int rows, int columns, Robot *robot)
{
    FILE *f = fopen(filePath, "r");
    char line[2048];

    if (f == NULL)
    {
        return 0;
    }

    for (int i = 0; i < 2; i++)
    {
        if (fgets(line, sizeof(line), f) == NULL)
        {
            fclose(f);
            return 0;
        }
    }

    for (int i = 0; i < rows; i++)
    {
        if (fgets(line, sizeof(line), f) == NULL)
        {
            break;
        }

        for (int j = 0; j < columns && line[j] != '\0' && line[j] != '\n' && line[j] != '\r'; j++)
        {
            robot->mapRobot[i][j] = line[j] - '0';
            robot->pheromoneGrade[i][j].pheromoneValue = 0;
        }
    }

    fclose(f);
    return 1;
}

int get_map_size(char *filePath, int *rows, int *columns)
{
    FILE *f = fopen(filePath, "r");
    if (!f)
        return 0;

    char line[4096];

    for (int i = 0; i < 2; i++)
    {
        if (fgets(line, sizeof(line), f) == NULL)
        {
            fclose(f);
            return 0;
        }
    }

    *rows = 0;
    *columns = 0;

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = '\0';

        int len = strlen(line);

        if (len == 0)
            continue;

        if (*columns == 0)
        {
            *columns = len;
        }
        else if (len != *columns)
        {
            printf("Erro: linhas com tamanhos diferentes!\n");
            fclose(f);
            return 0;
        }

        (*rows)++;
    }

    fclose(f);
    return 1;
}

int get_map_cyclos_robots(char *filePath, int *cyclos, int *robots)
{
    FILE *pont_arq;

    pont_arq = fopen(filePath, "r");

    if (pont_arq == NULL)
    {
        printf("Erro ao tentar abrir o arquivo de entrada de dados\n");
        exit(1);
    }

    fscanf(pont_arq, "%d%*c", &(*cyclos));
    fscanf(pont_arq, "%d", &(*robots));

    fclose(pont_arq);
}

void init_queue(CellQueue *q, int capacity)
{
    q->cells = (Cell *)malloc(capacity * sizeof(Cell));
    q->capacity = capacity;
    q->front = 0;
    q->tail = -1;
    q->size = 0;
}

void enqueue_cell(CellQueue *q, Cell cell)
{
    if (q->size == q->capacity)
    {
        q->front = (q->front + 1) % q->capacity;
        q->size--;
    }

    q->tail = (q->tail + 1) % q->capacity;
    q->cells[q->tail] = cell;
    q->size++;
}

void dequeue_cell(CellQueue *q)
{
    Cell c = q->cells[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
}

void print_queue(CellQueue *q)
{
    for (int i = 0; i < q->size; i++)
    {
        int index = (q->front + i) % q->capacity;
        printf("Cell %d = (%d, %d) pheromone: %.2f\n", i, q->cells[index].pos.x, q->cells[index].pos.y,
               q->cells[index].pheromoneValue);
    }
}