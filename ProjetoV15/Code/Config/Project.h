#ifndef PROJECT_H
#define PROJECT_H

typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    Position pos;
    double pheromoneValue;
    int mapValue;
} Cell;

typedef struct
{
    Cell *cells;
    int front;
    int tail;
    int size;
    int capacity;
} CellQueue;

typedef struct robot_link{
    int id;
    int check;
    long int hush;
    long int time_stamp;
    struct robot_link *prox;
} Robot_link;

typedef struct
{
    int id;
    Position pos;

    float drawRow;
    float drawCol;

    Cell **pheromoneGrade;
    int **mapRobot;
    int detectionRadius;
    Robot_link *link_list;
} Robot;

typedef struct message {
    int idRobot;
    long int time_stamp;
    Position pos;
    CellQueue pathMemory;
    struct message *prox;
} Message;

typedef struct
{
    int size;
    Message *msgs;
} Pool;

typedef struct
{
    int **value;
} RealMap;

#define CELL_UNKNOWN -1 
// Cell not yet discovered by the robot.
// Célula ainda desconhecida pelo robô.

#define CELL_FREE 0
// Free cell where the robot can move.
// Célula livre onde o robô pode andar.

#define CELL_VISITED 1
// Cell already visited/explored by the robot.
// Célula já visitada/explorada pelo robô.

#define CELL_WALL 2
// Wall or obstacle cell. The robot cannot move through it.
// Célula de parede/obstáculo. O robô não pode atravessar.

#define CELL_ROBOT 3
// Current cell occupied by the robot.
// Célula onde o robô está atualmente.

#define DETECTION_PHEROMONE_RADIUS 5
// Radius used by the robot to detect/analyze pheromone around its position.
// Raio usado pelo robô para detectar/analisar feromônio ao redor da posição atual.

#define MOVE_RADIUS 3
// Maximum movement radius. Defines how far the robot can choose a destination cell in one movement.
// Raio máximo de movimento. Define até onde o robô pode escolher uma célula de destino em um movimento.

#define DETECTION_COMUNICATION_RADIUS 3
// Communication detection radius. Defines the maximum distance for robots to exchange information.
// Raio de comunicação. Define a distância máxima para robôs trocarem informações.

#define MAX_NEIGHBORS ((2 * DETECTION_PHEROMONE_RADIUS + 1) * (2 * DETECTION_PHEROMONE_RADIUS + 1) - 1)
// Maximum number of neighboring cells inside the detection area, excluding the robot's own cell.
// Quantidade máxima de células vizinhas dentro da área de detecção, excluindo a própria célula do robô.

#define MEMORY_QUEUE_MAX 200
// Maximum capacity of the cell queue used to store path/history information.
// Capacidade máxima da fila de células usada para armazenar histórico/caminho.


#define PANEL_HEIGHT 80
#define MAX_LINE_SIZE 5096

#endif