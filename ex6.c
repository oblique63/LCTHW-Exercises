#include <stdio.h>

int main(int arg_count, char *arguments[]) {
  float power = 2.345f;
  double super_power = 56789.4532;

  char initial = 'A';
  char first_name[] = "Enrique";
  char last_name[] = "Gavidia";
  
  printf("You have %f levels of power.\n", power);
  printf("You have %f awesome super powers.\n", super_power);
  printf("My whole name is: %s %c. %s\n", first_name, initial, last_name);

  return 0;
}
