#include <stdio.h>

int main() {
  int age = 10;
  int height = 32; //fixed from uninitialized
  
  printf("I am %d years old.\n", age); //fixed from not providing age
  printf("I am %d inches tall.\n", height);
  
  return 0;
}
