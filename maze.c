#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char **argv){
    printf("%d", ParseArgs(argv, argc));
    return 0;
}

int ParseArgs(char **arguments, int argumentCount) {
  if(argumentCount<0)
    return 0;
  for(int i=1; i<argumentCount; i++){
    if(strcmp("--help", arguments[i]) == 0){
      PrintHelp();
      return 0;
    }
    else if (strcmp("--test", arguments[i]) == 0){
        if(++i>=argumentCount){
          fprintf(stderr, "Not enough arguments\n");
          return 0;
        }
        if(MazeTest(arguments[i]))
          printf("Valid\n");
        else
          printf("Invalid\n");
    }
    else if(strcmp("--lpath", arguments[i]) == 0){
      if(i+2>=argumentCount)
        return 0;
      int R = atoi(arguments[i+1]);
      int C = atoi(arguments[i+2]);
      return R+C;
    }
    else if(strcmp("--rpath", arguments[i]) == 0){
      if(i+2>=argumentCount)
        return 0;
      int R = atoi(arguments[i+1]);
      int C = atoi(arguments[i+2]);
      return R+C+1;
    }
  }
  return -1;
}

void PrintHelp(){
  printf("Available arguments: --help, --test, --rpath, --lpath\n");
  printf("--help shows this help menu\n");
  printf("--test will check any file for a valid maze definition, return Valid or Invalid\n");
  printf("--rpath R C needs two arguments R, and C, which will be the coordinates for the entry point into the maze, --rpath then looks for an exit using the right hand rule\n");
  printf("--lpath R C needs two arguments R, and C, which will be the coordinates for the entry point into the maze, --lpath then looks for an exit using the left hand rule\n");
}

bool MazeTest(char *arg){
  printf("File is: %s\n", arg);
  FILE *file;
  file = fopen(arg, "r");
  int rows = 0, cols = 0;
  char tmp;
  tmp = getc(file);

  while(tmp != ' ' && tmp != '\n' && tmp!= EOF){  //Loading rows and cols value from file into ints
    rows = rows*10;
    rows += atoi(&tmp);
    tmp = getc(file);       
  }
  tmp = getc(file);
  while(tmp != ' ' && tmp != '\n' && tmp!= EOF){
    cols = cols*10;
    cols += atoi(&tmp);
    tmp = getc(file);
  }
  if(rows<1 || cols<1)
    return false;

  int x = 0, y = 0;   //x=cols, y=rows
  tmp = getc(file);
  while(tmp != EOF){
    while(tmp != '\n' && tmp!= EOF){      //Check is map file contains bad values (>7/<0), not enough value, or too many values
      if(tmp != ' '){
        if(atoi(&tmp)>7 || atoi(&tmp)<0)                    //Still accepts letters!!! FIXME!!
          return false;
        x++;
      }
      tmp = getc(file);
    }
    y++;
    if(x!=cols)
      return false;
    tmp = getc(file);
    x=0;
  }
  if(y!=rows)
      return false;
  return true;
}

int MapCtor(Map *map, int cols, int rows){
  map->cols=cols;
  map->rows=rows;
  size_t mapsize = rows*cols*sizeof(char);
  if(mapsize!=0){
    map->cells=malloc(mapsize);
    if(map->cells==NULL)
      return 0;
  }
  else
    return 0;
  return 1;
}

void MapDtor(Map *map){
  free(map->cells);
  map->cells=NULL;
  map->cols=0;
  map->rows=0;
}


