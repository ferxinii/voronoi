#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/fortune.h"
#include "include/plot.h"


/* CURRENT STATE
 *  Right now, the code only produces and processes the correct 
 *  events, as well as making a cool video. However, the real
 *  voronoi diagram is not being constructed through the DCEL, 
 *  and some errors regarding vertex events are found
 */

/* KNOWN PROBLEMS
 *  [] Vertex events are sometimes not created, or the incorrect
 *     arc disappears, note with higher number of seeds.
 *  [] If a given seed is very close to a circumcircle traced by 
 *     three adjacent arcs (a distance of EPS to the circle), then 
 *     the vertex event that should happen will not be created. This
 *     happens in the function circle_contains_seeds, queue.c
 *  [] The introduction of EPS allows for correct comparison of
 *     points, but may lead to errors if events are very close to
 *     each other.
 */

/* TODO
 * [?] Fix knwown problem 1
 * [ ] Check if gnuplot and ffmpeg are installed before running
 * [ ] Work on DCEL to construct real voronoi diagram...
 * [ ] Make code more efficient: Use binary trees istead of linked
 *     lists for the beachline.
 */


int main(int argc, char **argv)
{
  srand(time(NULL));
  
  int N = 5;
  if (argc == 2) {
    N = atoi(argv[1]);
  } else if (argc > 2) {
    printf("Too many arguments!!\n Only input the number of seeds\n");
    exit(1);
  }
  point2D_T *seeds = random_seeds(1, N);
  write_seeds_txt(seeds, N, "seeds.txt");
  //plot_seeds(seeds, N);

  fortune_algorithm(seeds, N); 
  
  free(seeds);
}

