#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/fortune.h"
#include "include/plot.h"


int main(void)
{
  srand(time(NULL));

  int N = 10;
  point2D_T *seeds = random_seeds(1, N);
  write_seeds_txt(seeds, N, "seeds.txt");
  //plot_seeds(seeds, N);

  fortune_algorithm(seeds, N); 
  
  free(seeds);
}

