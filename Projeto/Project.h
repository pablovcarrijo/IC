typedef struct{
    int i;
    int j;
} Position;

typedef struct{
    int id;
    Position pos;
} Robot;

typedef struct{
    int **mapGrade;
    double **pheromoneGrade;
    Robot *robot;
    int robotCount;
} Map;


void init_map(int rows, int columns, Map *grade);
void robotsMap(int quantity, int rows, int columns, Map *map);
void robotMove(int rows, int columns, Map *grade);
void viewMap(int rows, int columns, Map grade);
void pheromone_evaporate(int rows, int columns, Map *grade, int percentage);