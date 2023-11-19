#define TYPES_H

typedef struct{
    int R;
    int C;
} Maze;

void PrintHelp();
int ParseArgs(char **arguments, int argumentCount);