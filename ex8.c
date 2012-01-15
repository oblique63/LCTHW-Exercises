#include <stdio.h>

int main(int argc, char *argv[]) {
  int areas[] = {10, 12, 13, 14, 20};
  char name[] = "Enrique";
  char full_name[] = {
    'E','n','r','i','q','u','e',' ',
    'G','a','v','i','d','i','a','\0'
  };
  
  printf("size of an int: %ld\n", sizeof(int));
  printf("size of areas (int[]): %ld\n", sizeof(areas));
  printf("number of ints in areas: %ld\n", sizeof(areas)/sizeof(int));
  
  printf("size of a char: %ld\n", sizeof(char));
  printf("size of name (char[]): %ld\n", sizeof(name));
  printf("number of chars: %ld\n", sizeof(name)/sizeof(char));
  
  printf("size of full_name (char[]): %ld\n", sizeof(full_name));
  printf("number of chars: %ld\n", sizeof(full_name)/sizeof(char));
  
  printf("name=\"%s\" and full_name=\"%s\"\n", name, full_name);

  return 0;
}
