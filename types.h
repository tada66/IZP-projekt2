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