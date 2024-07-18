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
  
  
  focus_T f1 = {2, 2};
  focus_T f2 = {2, 2};
  double k = 0.5;

  roots2_T x = intersect_parabs(f1,f2,k);
  double y1 = parabola(f1, k, x.pos); 
  double y2 = parabola(f2, k, x.neg); 
  double y3 = parabola(f1, k, x.pos); 
  double y4 = parabola(f2, k, x.neg);

  printf("x: %f, y1: %f, y2: %f\n", x.pos, y1, y2);
  printf("x: %f, y3: %f, y4: %f\n", x.neg, y3, y4);

  
  free(seeds);
}
