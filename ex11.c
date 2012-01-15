#include <stdio.h>

int main(int argc, char *argv[]) {
  int i = 0;
  while (i < argc) {
    printf("arg %d: %s\n", i, argv[i]);
    i++;
  }

  int num_states = 4;
  char *states[] = {
    "California", "Oregon", "Washington", "Texas"
  };
  
  i = num_states;
  while (i > 0) {
    i--;
    printf("state %d: %s\n", i, states[i]);
  }
  
  return 0;
}
