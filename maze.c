#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int ParseArgs(char **arguments, int argumentCount) {
  if(argumentCount<0)
    return 0;
  for(int i=1; i<argumentCount; i++){
    if(strcmp("--help", arguments[i]) == 0){
      return 0;
    }
    else if (strcmp("--test", arguments[i]) == 0){
        return 1;
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
int main(int argc, char **argv){
    printf("%d", ParseArgs(argv, argc));
    return 0;
}