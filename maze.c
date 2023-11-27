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
      if(argumentCount>2){
        fprintf(stderr, "Too many arguments!\n");
        return -1;
      }
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
    else if(strcmp("--isb", arguments[i]) == 0){
      if(i+4>=argumentCount)
        return 0;
      Map map;
      if(!MapInit(&map, arguments[i+1]))
        return -1;
      int R = atoi(arguments[i+2]);
      int C = atoi(arguments[i+3]);
      int b = atoi(arguments[i+4]);
      printf("is border: %d\n", isborder(&map, R, C, b));
      return -2;
    }
    else if(strcmp("--stb", arguments[i]) == 0){
      if(i+4>=argumentCount)
        return 0;
      Map map;
      if(!MapInit(&map, arguments[i+1]))
        return -1;
      int R = atoi(arguments[i+2]);
      int C = atoi(arguments[i+3]);
      int lr = atoi(arguments[i+4]);
      Mazefollower(&map, R, C, 0, lr);
      return -2;
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
  printf("----------DEBUG ONLY----------\n");
  printf("--isb test for isborder, needs maze file, r, c, border to check - 0 for bottom-top, 1 for right, 2 for left\n");
  printf("--stb test for start_border, needs maze file, r, c, direction to go - 0 for left, 1 for right\n");
}

bool MapTest(Map *map){                 //TODO: check if some arguments are missing
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
        if((map->cells[(map->cols*i)+j]&1)!=1)    //we dont want something ...xxxx1
          return false;
      }
      else
        if(j!=0)
          if((map->cells[(map->cols*i)+j]&1)==1)
            return false;
      if((map->cells[(map->cols*i)+j]&2)==2)      //we look for something ...xxx1x
        WallFound=true;
      else
        WallFound=false;
    }
  }
  WallFound=false;                    //Now we go check vertical walls
  for(int i=0; i<map->cols; i++){    //We flip the map 90° for the scan
    for(int j=0; j<map->rows; j++){
      if(j==0 && (i&1)==0)  //Board is staggered, no need to check for first in some cases (0, 2, 4,...)
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
  4=check top/bottom border
  2=check right border
  1=check left border
  */
  if(border<1 || border>4 || border==3){
    fprintf(stderr, "Error invalid value of border argument in isborder!\n");
    return false;
  }
  if(!FitsInMap(map, r, c))
    return false;
  if(border==4){//checking the top/bottom border - bitwise 4 op - xxxx1xx
    if((map->cells[(r*map->cols)+c]&4)==4)
      return true;
  }
  else if(border==2){
    if((map->cells[(r*map->cols)+c]&2)==2)
      return true;
  }
  else{
    if((map->cells[(r*map->cols)+c]&1)==1)
      return true;
  }
  return false;
}

bool FitsInMap(Map *map, int r, int c){ //Checks whether the r and c coordinates are valid and fit in the maze defined
  if(map==NULL)
    return false;
  if(r>=map->rows || r<0){
    fprintf(stderr, "Invalid value of rows! Input=%d Maximum=%d\n", r, map->rows-1);
    return false;
  }
  if(c>=map->cols || c<0){
    fprintf(stderr, "Invalid value of cols! Input=%d Maximum=%d\n", c, map->cols-1);
    return false;
  }
  return true;
}

int start_border(Map *map, int r, int c, int leftright){ 
  /*leftright = direction to go 
      0=follow lefthand rule
      1=follow righthand rule
    returns =
      -1=entry was not possible
      0=go down
      1= go up
      2= go left
      3= go right
  */
  if(leftright<0 || leftright>1){
    fprintf(stderr, "Wrong value of leftright in start_border!\n");
    return -1;
  }
  if(!FitsInMap(map, r, c))
    return -1;
  int rotation =-1;
  /* 0=going down (entering from top)
    1=going up    (entering from bottom)
    2=going left  (entering from right)
    3=going right (entering from left)
  */
  if(c==0)                //Entering from left
    if((map->cells[r*map->cols]&1)!=1)
      rotation=3;
  if((c+1)==map->cols)    //entering from right
    if((map->cells[r*map->cols+c]&2)!=2)
      rotation=2;
  if(r==0)                //entering from top
    if((map->cells[c]&4)!=4)
      rotation=0;
  if((r+1)==map->rows)    //entering from bottom
    if((map->cells[r*map->cols+c]&4)!=4)
      rotation=1;
  if(rotation==-1){
    fprintf(stderr, "Entry was not possible from specified coordinates (%d, %d)!\n", r, c);
    return -1;
  }
  if(leftright==0){       //left hand rule
    //if(((r+1)&1)==1){
      printf("rotation: %d\n", rotation);
    //}
  }
  else{                   //right hand rule
    if(rotation==3){
      if((r&1)==0){ //Offset by one, we start at 0 so even lines became odd, and reverse
        printf("is border2: %d\n", isborder(map, r, c, 2)); //odd
        printf("is border4: %d\n", isborder(map, r, c, 4));
        printf("is border1: %d\n", isborder(map, r, c, 1));
      }
      else{
        printf("is border4: %d\n", isborder(map, r, c, 4));
        printf("is border2: %d\n", isborder(map, r, c, 2));
        printf("is border1: %d\n", isborder(map, r, c, 1));
      }
      //check 2 4 1 on odd rows
      //check 4 2 1 on even rows
    }
    if(rotation==2){
      //check 4 1 2 on odd rows
      //check 1 4 2 on even rows
    }
    if(rotation==0){
      //check 1 2 4
    }
    if(rotation==1){
      //check 2 1 4
    }
  }
  return 0;
}

int next_rotation(Map *map, int r, int c, int leftright, int rotation){
  int priority[3];
  if(rotation==0){
    priority[0] = 1;
    priority[1] = 2;
    priority[2] = 4;
  }
  else if(rotation==1){
    priority[0] = 2;
    priority[1] = 1;
    priority[2] = 4;
  }
  else{
    if(rotation==3){
      priority[0] = 4;
      priority[1] = 2;
      priority[2] = 1;
    }
    if(rotation==2){
      priority[0] = 1;
      priority[1] = 4;
      priority[2] = 2;
    }
    if(((r+c)%2)==0){   /* Cell shape \/ */ 
      priority[0] = priority[0] + priority[1];
      priority[1] = priority[0] - priority[1];
      priority[0] = priority[0] - priority[1];
    }
  }
  if(!leftright){//left hand rule
    priority[0] = priority[0] + priority[1];
    priority[1] = priority[0] - priority[1];
    priority[0] = priority[0] - priority[1];
  }
  for(int i=0; i<3; i++)
    if(!isborder(map, r, c, priority[i]))
      return priority[i];

  return -1;
}

void Mazefollower(Map *map, int r, int c, int leftright, int rotation){
  while(FitsInMap(map, r, c)){
    printf("%d %d\n", r+1, c+1);
    int wall = next_rotation(map, r, c, leftright, rotation);
    if(wall==1){
      c-=1;
      rotation=2;
    }
    if(wall==2){
      c+=1;
      rotation=3;
    }
    if(wall==4){
      if(((r+c)%2)==0){
        r-=1;
        rotation=1;
      }
      else{
        r+=1;
        rotation=0;
      }
    }
  }
}