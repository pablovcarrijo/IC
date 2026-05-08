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

#define CELL_FREE 0
#define CELL_VISITED 1
#define CELL_WALL 2
#define CELL_ROBOT 3
#define DETECTION_PHEROMONE_RADIUS 1
#define DETECTION_COMUNICATION_RADIUS 4
#define MAX_NEIGHBORS ((2 * DETECTION_PHEROMONE_RADIUS + 1) * (2 * DETECTION_PHEROMONE_RADIUS + 1) - 1)
#define QUEUE_MAX 200

#endif