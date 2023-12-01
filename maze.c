#include <stdio.h>    //printf, fprintf, getc, fopen, fclose
#include <stdlib.h>   //atoi, malloc, free
#include <string.h>   //strcmp
#include <stdbool.h>  //bool  

typedef struct {  //define the map
  int rows;
  int cols;
  unsigned char *cells;
} Map;

//START function prototypes
void PrintHelp();
int ParseArgs(char **arguments, int argumentCount);
bool MapTest(Map *map);
bool MapCtor(Map *map, int cols, int rows);
void MapDtor(Map *map);
bool MapInit(Map *map, char* arg);
bool isborder(Map *map, int r, int c, int border);
int start_border(Map *map, int r, int c, int leftright);
bool FitsInMap(Map *map, int r, int c);
int next_rotation(Map *map, int r, int c, int leftright, int rotation);
void Mazefollower(Map *map, int r, int c, int leftright, int rotation);
//END function prototypes

int main(int argc, char **argv){
  int parse = ParseArgs(argv, argc);
  if(parse==-3)
    fprintf(stderr, "Invalid argument! See --help for help\n");
  else if(parse==-2)
    fprintf(stderr, "Too few arguments!\n");
  else if(parse==-1)
    fprintf(stderr, "Too many arguments!\n");
  else if(parse==1)
    PrintHelp(0);
  else{
    Map map;
    if(parse==2){   //MapTest
      if(MapInit(&map, argv[2])){
        if(MapTest(&map))
          printf("Valid\n");
        else
          printf("Invalid\n");
        MapDtor(&map);
      }
      else
        printf("Invalid\n");
      return 0;
    }
    if(MapInit(&map, argv[4])){
      int r = atoi(argv[2])-1;
      int c = atoi(argv[3])-1;
      bool isRight = false;
      if(parse==4)
        isRight=true;
      int rotation = start_border(&map, r, c, isRight);
      if(rotation!=-1)
        Mazefollower(&map, r, c, isRight, rotation);
      else{
        MapDtor(&map);
        return 0;
      }
      MapDtor(&map);
    }
  }
  return 0;
}

int ParseArgs(char **arguments, int argumentCount) {
  if(argumentCount<2)
    return -2;
  bool isRight = false;
  if(strcmp("--help", arguments[1]) == 0){
    if(argumentCount>2)
      return -1;    //Too may arguments
    return 1;       //PrintHelp()
  }
  else if (strcmp("--test", arguments[1]) == 0){
    if(2>=argumentCount)
      return -2;    //Too few arguments
    return 2;       //MapTest()
  }
  else if(strcmp("--lpath", arguments[1]) == 0){
    isRight=false;
  }
  else if(strcmp("--rpath", arguments[1]) == 0){
    isRight=true;
  }
  else
    return -3;    //Invalid argument
  if(3>=argumentCount)
    return -2;    //Too few arguments
  else if(5<argumentCount)
    return -1;    //Too many arguments
  if(isRight)
    return 4; //rpath
  else
    return 3; //lpath
}

void PrintHelp(){  //Called on --help argument, displays the help text
  printf("Available arguments: --help, --test, --rpath, --lpath\n");
  printf("--help shows this help menu\n");
  printf("--test will check any file for a valid maze definition, return Valid or Invalid. Example usage: '--test bludiste.txt'\n");
  printf("--rpath R C needs two coordinates - R, and C, which will be the coordinates for the entry point into the maze, then the maze definition file, --rpath then looks for an exit using the right hand rule\n");
  printf("--lpath R C needs two coordinates - R, and C, which will be the coordinates for the entry point into the maze, then the maze definition file, --lpath then looks for an exit using the left hand rule\n");
}

bool MapTest(Map *map){ //Checks for the correct definition of the maze map
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

bool MapInit(Map *map, char* arg){  //Stores the map into memory from file
  FILE *file;
  file = fopen(arg, "r");
  if(file==NULL){
    fprintf(stderr, "Error loading file %s! Does it exist?\n", arg);
    return false;   //No need for fclose because file failed to open and therefore there isn't anything to close, ie will result in segfault
  }
  int rows = 0, cols = 0;
  char tmp;
  tmp = getc(file);

  while(rows==0 || (tmp != ' ' && tmp != '\n' && tmp!= EOF)){
    if((tmp != ' ' && tmp != '\n' && tmp!= EOF) && (tmp<'0' || tmp>'9'))
      return false;
    rows = rows*10;
    rows += atoi(&tmp);
    tmp = getc(file);       
  }
  tmp = getc(file);
  while(cols==0 || (tmp != ' ' && tmp != '\n' && tmp!= EOF)){
    if((tmp != ' ' && tmp != '\n' && tmp!= EOF) && (tmp<'0' || tmp>'9'))
      return false;
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
          map->cells[i]=tmp;  //TODO: make check past rows*cols just in case the definition in file is too large
          i++;
          if(i>rows*cols){
            fclose(file);
            return false;
          }
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

bool isborder(Map *map, int r, int c, int border){//Check whether a side has a border or no, returns true on border found
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
  if(r>=map->rows || r<0)
    return false;

  if(c>=map->cols || c<0)
    return false;
  return true;
}

int start_border(Map *map, int r, int c, int leftright){  //Checks whether entry into maze is possible, if it is function returns the next rotation
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
    if(!isborder(map, r, c, 1))
      rotation=3;
  if((c+1)==map->cols)    //entering from right
    if(!isborder(map, r, c, 2))
      rotation=2;
  if(r==0)                //entering from top
    if((c&1)==0)
      if(!isborder(map, r, c, 4))
        rotation=0;
  if((r+1)==map->rows)    //entering from bottom
    if((((r+c)%2)!=0))    /* Cell shape /\ */
      if(!isborder(map, r, c, 4))
        rotation=1;
  if(rotation==-1){
    fprintf(stderr, "Entry was not possible from specified coordinates (%d, %d)!\n", r+1, c+1);
    return -1;
  }
  return rotation;
}

int next_rotation(Map *map, int r, int c, int leftright, int rotation){  //Defines which sides are checked first based on the rotation, then returns next rotation based on which sides don't have a border 
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

void Mazefollower(Map *map, int r, int c, int leftright, int rotation){//  Called after start_border, iterates next_rotation until we get out of the maze
  while(FitsInMap(map, r, c)){
    printf("%d,%d\n", r+1, c+1);
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