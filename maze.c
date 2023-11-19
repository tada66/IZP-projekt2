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
    }/*
    else if(strcmp("-n", arguments[i]) == 0){
      if(++i>=argumentCount)
        return 0;
      if(config->nValue!=10)
        return 0;
      config->nValue=atoi(arguments[i]);
    }
    else if(strcmp("-s", arguments[i]) == 0){
      if(++i>=argumentCount)
        return 0;
      if(config->sValue!=(NULL))
        return 0;
      config->sValue=arguments[i];
    }*/
  }
  return 2;
}
int main(int argc, char **argv){
    Printf("%d", ParseArgs(argv, argc))
    return 0;
}