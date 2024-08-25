#include "include/plot.h"
#include "include/geometry.h"
#include <stdio.h>
#include <stdlib.h>


FILE *popen_gnuplot(void)
{
  FILE *pipe = popen("gnuplot -persistent", "w");
  if (!pipe) {
    printf("ERROR! Cannot open gnuplot pipe...\n");
    exit(1);
  }

  fprintf(pipe, "set terminal png enhanced font 'Arial,18' size 1080,1080 enhanced \n");
  fprintf(pipe, "set output './plot.png'\n");
  fprintf(pipe, "set xrange [0:1]\n");
  fprintf(pipe, "set yrange [0:1]\n");
  fprintf(pipe, "set size square\n");

  return pipe;
}


void start_plot(FILE *pipe)
{
  fprintf(pipe, "");
  fprintf(pipe, "plot ");
}


void end_plot(FILE *pipe)
{
  fprintf(pipe, "\n");
  fclose(pipe);
}


void add_point(FILE *pipe, point2D_T point, const char *config)
{
  fprintf(pipe, "\"<echo \'%f %f\'\" w p %s notitle, ", point.x, point.y, config);
}


void add_seeds(FILE *pipe, const point2D_T *seeds, int N)
{ 
  for (int ii=0; ii<N; ii++) {
    add_point(pipe, seeds[ii], "pt 7 ps 3 lc 'black'");
  }
}


void add_circle(FILE *pipe, point2D_T center, double radius)
{
  fprintf(pipe, "\"<echo \'%f %f %f\'\" w circles lc 'red' notitle, ", center.x, center.y, radius); 
}


void plot_seeds(const point2D_T *seeds, int N)
{
  FILE *pipe = popen_gnuplot();
  start_plot(pipe);
  add_seeds(pipe, seeds, N);
  end_plot(pipe);
  
}

