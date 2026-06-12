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

typedef struct
{
    int id;
    Position pos;
    Cell **pheromoneGrade;
    int **mapRobot;
    int detectionRadius;
} Robot;

typedef struct
{
    int idRobot; 
    Position pos;   
    CellQueue pathMemory;
} Pool;

#define CELL_FREE 0
#define CELL_VISITED 1
#define CELL_WALL 2
#define CELL_ROBOT 3
#define DETECTION_PHEROMONE_RADIUS 1
#define DETECTION_COMUNICATION_RADIUS 3
#define MAX_NEIGHBORS ((2 * DETECTION_PHEROMONE_RADIUS + 1) * (2 * DETECTION_PHEROMONE_RADIUS + 1) - 1)
#define QUEUE_MAX 10

void init_robots(int id, int rows, int columns, Robot *robot, Pool *pool);
void place_robot_random(int rows, int columns, Robot *robot);

void robot_move(int rows, int columns, Robot *robot, Pool *pool, int id);
void robot_comunication(int rows, int columns, int quantityRobots, Robot *robot, Robot *robots, Pool *pool);
void view_pheromone_robot_map(int rows, int columns, Robot *robot, Pool *pool);
void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta);

int load_map_from_txt(const char *filePath, int rows, int columns, Robot *robot);
int get_map_size(char *filePath, int *rows, int *columns);
int get_map_cyclos_robots(char *filePath, int *cyclos, int *robots);

void init_queue(CellQueue *q, int capacity);
void enqueue_cell(CellQueue *q, Cell cell);
void dequeue_cell(CellQueue *q);
void print_queue(CellQueue *q);