#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/fortune.h"


int main(void)
{
  srand(time(NULL));

  int N = 10;
  point2D_T *seeds = random_seeds(1, N);

  fortune_algorithm(seeds, N); 
  
  free(seeds);
}
