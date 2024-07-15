#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/fortune.h"



int main(void)
{
  srand(time(NULL));
  int N = 10;
  seed_T *seeds = random_seeds(1, N);

  fortune_algorithm(seeds, N); 
  
  /*
  focus_T f1 = {2, 2};
  focus_T f2 = {3, 2};
  double k = 1.2;

  double x = x_intersection(f1,f2,k);
  double y1 = parabola_y(f1, k, x); 
  double y2 = parabola_y(f2, k, x); 

  printf("x: %f, y1: %f, y2: %f\n", x, y1, y2);
  */
  free(seeds);
}
