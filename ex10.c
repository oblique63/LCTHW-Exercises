#include <stdio.h>

int main(int argc, char *argv[]) {
  int i = 0;

  int num_states = 5;
  char *states[] = {
    "California", "Oregon", NULL, "Washington", "Texas"
  };

  argv[0] = states[0];
  states[1] = argv[1];

  for (i=1; i < argc; i++) {
    printf("arg %d: %s\n", i, argv[i]);
  }

  for (i=0; i < num_states; i++) {
    printf("state %d: %s\n", i, states[i]);
  }

  return 0;
}
