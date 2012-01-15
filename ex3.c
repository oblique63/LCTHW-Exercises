#include <stdio.h>

int main() {
  int age = 23;
  int height = 72;

  int padding = 4;
  
  printf("I am %*d  years old.\n", padding, age);
  printf("I am %-*d inches tall.\n", padding, height);

  return 0;
}
