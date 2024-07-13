#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/fortune.h"



int main(void)
{
  srand(time(NULL));
  int N = 10;
  seed_T *seeds = random_seeds(1, N);

  for (int ii=0; ii<N; ii++) {
    printf("%d -->  x : %f, y : %f\n", ii, seeds[ii].x, seeds[ii].y);
  }

  putchar('\n');

  event_T *queue = initialize_queue(seeds, N);
  print_queue(queue);
  
}
