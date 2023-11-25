#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>      //TODO Pozor!!! nespolehat se ze system uvolni vsechnu pamet, program musi pamet uvolnit sam

int main(int argc, char **argv){
  ParseArgs(argv, argc);
  return 0;
}
void MapPrint(Map *map){    //This function prints the whole map, used for debugging, remove for release
  printf("Rows: %d\nCols: %d\n", map->rows, map->cols);
  for(int i=0; i<(map->cols*map->rows); i++){
    printf("%c ", map->cells[i]);
    if((i+1)%map->cols==0)
      printf("\n");
  }
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
        Map map;
        if(!MapInit(&map, arguments[i]))
          return -1;
        //MapPrint(&map);                           //TODO: remove; DEBUG only
        if(MapTest(&map))
          printf("Valid\n");
        else
          printf("Invalid\n");
        MapDtor(&map);
    }
    else if(strcmp("--lpath", arguments[i]) == 0){
      if(i+2>=argumentCount)
        return 0;
      int R = atoi(arguments[i+1]);
      int C = atoi(arguments[i+2]);
      Map map;
      if(MapInit(&map, arguments[i+3]))
        return 1;
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

bool MapTest(Map *map){
  if(map->cols<1 || map->rows<1)
    return false;
  for(int i=0; i<(map->cols*map->rows); i++){
    if(map->cells[i]>'7'  || map->cells[i]<'0')
      return false;
  }
  bool WallFound = false;
  for(int i=0; i<map->rows; i++){   //First check the walls in rows, ie left and right
    WallFound=false;
    for(int j=0; j<(map->cols); j++){
      if(WallFound){
        if((map->cells[(map->cols*i)+j]&1)!=1)    //we look for something xxxx1
          return false;
      }
      else
        if(j!=0)
          if((map->cells[(map->cols*i)+j]&1)==1)
            return false;
      if((map->cells[(map->cols*i)+j]&2)==2)      //we look for something xxx1x
        WallFound=true;
      else
        WallFound=false;
    }
  }
  WallFound=false;                    //Now we go check vertical walls
  for(int i=0; i<map->cols; i++){    //We flip the map 90° for the scan
    for(int j=0; j<map->rows; j++){
      if(j==0 && (i&1)==0)  //Board is staggered, no need to check for first in some cases (0, 2, 4)
        j++;
      if((map->cells[(map->cols*j)+i]&4)==4){
        if(((j+i)%2)!=0)    /* Cell shape /\ */ 
          WallFound=true;
        else{               /* Cell shape \/ */
          if(!WallFound)
            return false;
          WallFound=false;
        }
      }
      else{
        if(WallFound)
          return false;
        WallFound=false;
      }
    }
    WallFound=false;
  }
  return true;
}

bool MapCtor(Map *map, int cols, int rows){//returns 0 upon encountering an error with allocating memory
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

bool MapInit(Map *map, char* arg){
  FILE *file;
  file = fopen(arg, "r");
  if(file==NULL){
    fprintf(stderr, "Error loading file %s! Does it exist?\n", arg);
    return false;   //No need for fclose because file failed to open and therefore there isn't anything to close, ie will result in segfault
  }
  int rows = 0, cols = 0;
  char tmp;
  tmp = getc(file);

  while(tmp != ' ' && tmp != '\n' && tmp!= EOF){
    while(tmp>'9'||tmp<'0')   //CAUTION
      tmp=getc(file);         //For some reason there is '-ne' at the start of the official tests, so idk this should make the map load
    rows = rows*10;           //Those tests are weird though, for some reason they also expect the program to do a double new line (maybe be CRLF issue?)
    rows += atoi(&tmp);
    tmp = getc(file);       
  }
  tmp = getc(file);
  while(tmp != ' ' && tmp != '\n' && tmp!= EOF){
    cols = cols*10;
    cols += atoi(&tmp);
    tmp = getc(file);
  }

  if(!MapCtor(map, cols, rows)){
    fprintf(stderr, "Error allocating memory!\n");
    fclose(file);
    return false;
  }
  tmp = getc(file);
  int i = 0;
  while(tmp != EOF){
    while(tmp != '\n' && tmp!= EOF){
      if(tmp != ' '){
        if(map->cells!=NULL){
          map->cells[i]=tmp;
          i++;
        }
        else
          return false;
      }
      tmp = getc(file);
    }
    tmp = getc(file);
  }
  fclose(file);
  return true;
}

bool isborder(Map *map, int r, int c, int border){
  /*int border behaviour 
  0=check top/bottom border
  1=check right border
  2=check left border
  */
  
  return false;
}