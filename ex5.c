#include <stdio.h>

int main(int argc, char *argv[]) {
  int distance = 100;

  printf("You are %d miles away.\n", distance);

  printf("argc: %d\n", argc);
  printf("argv: ");
  
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  
  printf("\n");

  return 0;
}
