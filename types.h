#include <stdbool.h>
#define TYPES_H

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

void PrintHelp();
int ParseArgs(char **arguments, int argumentCount);
bool MazeTest(char *file);
int MapCtor(Map *map, int cols, int rows);
void MapDtor(Map *map);
int MapInit(Map *map, char* arg);
bool isborder(Map *map, int r, int c, int border);
int start_border(Map *map, int r, int c, int leftright);