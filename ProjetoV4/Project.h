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

#define CELL_FREE 0
#define CELL_VISITED 1
#define CELL_WALL 2
#define CELL_ROBOT 3

void init_robots(int rows, int columns, Robot *robot);
void place_robot_random(int rows, int columns, Robot *robot);

void robot_move(int rows, int columns, Robot *robot);
void view_pheromone_robot_map(int rows, int columns, Robot *robot);
void pheromone_robot_evaporate(Robot *robot, int rows, int columns, double beta);

int load_map_from_txt(const char *filePath, int rows, int columns, Robot *robot);
int get_map_size(char *filePath, int *rows, int *columns);
int get_map_cyclos_robots(char *filePath, int *cyclos, int *robots);