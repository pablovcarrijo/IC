typedef struct{
    int x;
    int y;
} Position;

typedef struct{
    int id;
    Position pos;
    double **pheromoneGrade;
} Robot;

typedef struct{
    int **mapGrade;
    Robot *robot;
    int robotCount;
} Map;


void init_map(int rows, int columns, Map *grade);
void robotsMap(int quantity, int rows, int columns, Map *map);
void robotMove(int rows, int columns, Map *grade);
void viewMap(int rows, int columns, Map *map);
void pheromone_evaporate(Robot *robot, int rows, int columns, double beta);
void viewRobotMap(int rows, int columns, Robot *robot);