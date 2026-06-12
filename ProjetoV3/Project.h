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
void view_map(int rows, int columns, Map *map);

void robots_map(int quantity, int rows, int columns, Map *map);
void copy_map_to_robot(Robot *robot, Map *grade, int rows, int columns);
void robot_move(int rows, int columns, Map *grade);
void view_robot_map(int rows, int columns, Robot *robot);
void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta);

void global_pheromone_evaporate(Map *grade, int rows, int columns, double beta);
void view_pheromone_map(int rows, int columns, Map *map);

int load_map_from_txt(const char *filePath, int rows, int columns, Map *grade);
int get_map_size(char *filePath, int *rows, int *columns);
int get_map_cyclos_robots(char *filePath, int *cyclos, int *robots);