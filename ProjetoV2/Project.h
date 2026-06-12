typedef struct{
    int x;
    int y;
} Position;

typedef struct{
    int id;
    Position pos;
    double **pheromoneGrade;
    int **mapRobot;
} Robot;

typedef struct{
    int **mapGrade;
    Robot *robot;
    int robotCount;
    double **pheromoneGrade;
} Map;

#define CELL_FREE 0
#define CELL_VISITED 1
#define CELL_WALL 2
#define CELL_ROBOT 3

void init_map(int rows, int columns, Map *grade);
int load_map_from_txt(const char *filePath, int rows, int columns, Map *grade);
void robotsMap(int quantity, int rows, int columns, Map *map);
void robotMove(int rows, int columns, Map *grade);
void viewMap(int rows, int columns, Map *map);
void viewPheromoneMap(int rows, int columns, Map *map);
void pheromone_evaporate(Robot *robot, Map *grade, int rows, int columns, double beta);
void viewRobotMap(int rows, int columns, Robot *robot);
