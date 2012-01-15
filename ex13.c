#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    puts("ERROR: You need at least one argument.");
    return 1;
  }
  
  
  int i;
  for (i = 1; i < argc; i++) {

    int j;
    char letter;

    for (j = 0, letter = argv[i][0]; letter != '\0'; j++, letter = argv[i][j]) {
      
      switch (letter) {
          case 'a':
          case 'A':
              printf("%d-%d: 'A'\n", i,j);
              break;
          
          case 'e':
          case 'E':
              printf("%d-%d: 'E'\n", i,j);
              break;
          
          case 'i':
          case 'I':
              printf("%d-%d: 'I'\n", i,j);
              break;
    
          case 'o':
          case 'O':
              printf("%d-%d: 'O'\n", i,j);
              break;
    
          case 'u':
          case 'U':
              printf("%d-%d: 'U'\n", i,j);
              break;
              
          case 'y':
          case 'Y':
              if (j >= 2) {
                printf("%d-%d: 'Y'\n", i,j);
                break;
              }
              //break;

          default:
              printf("%d-%d: %c is not a vowel\n", i, j, letter);
      }
    }
  }
  return 0;
}
