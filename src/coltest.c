#include "c_cool.h"

int main() {
  int     i = 0;
  char    achar;

  for(i = 0; i < 50; i++) {
    if(i != 12) {
      outputcolor(i, B_WHITE);
      printf("Hello there! : %d\n", i);
    }
    if(i % 10 == 0)
      scanf("%c", &achar);
  }

// outputcolor(33,B_BLUE);
// printf("Hello there! : %d\n",i);

  outputcolor(1, B_BLUE);
  printf("Hello there! : %d\n", i);

  outputcolor(FH_WHITE, BH_BLUE);
  printf("Hello there! : %d\n", i);

  return 0;

}
