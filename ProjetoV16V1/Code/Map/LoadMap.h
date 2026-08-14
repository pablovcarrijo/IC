#ifndef LOAD_MAP_H
#define LOAD_MAP_H

int **allocate_matrix(int rows, int cols, int initialValue);

void free_matrix(int **matrix, int rows);

void remove_line_break(char *line);

int load_map_from_txt( const char *filePath, int *maxCycles, int *quantityRobots, int *rows, int *cols, int ***map );

#endif