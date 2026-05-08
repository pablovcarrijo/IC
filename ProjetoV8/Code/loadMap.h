#include "Project.h"

int load_map_from_txt(const char *filePath, int rows, int columns, Robot *robot);
int get_map_size(char *filePath, int *rows, int *columns);
int get_map_cyclos_robots(char *filePath, int *cyclos, int *robots);
