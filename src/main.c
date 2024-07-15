#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/fortune.h"



int main(void)
{
  srand(time(NULL));
  int N = 10;
  seed_T *seeds = random_seeds(1, N);

  //fortune_algorithm(seeds, N); 
  
  
  focus_T f1 = {2, 2};
  focus_T f2 = {2, 2};
  double k = 0.5;

  parab_intersect_T x = x_intersection(f1,f2,k);
  double y1 = parabola_y(f1, k, x.x_left); 
  double y2 = parabola_y(f2, k, x.x_left); 
  double y3 = parabola_y(f1, k, x.x_right); 
  double y4 = parabola_y(f2, k, x.x_right);

  printf("x: %f, y1: %f, y2: %f\n", x.x_left, y1, y2);
  printf("x: %f, y3: %f, y4: %f\n", x.x_right, y3, y4);

  
  free(seeds);
}
