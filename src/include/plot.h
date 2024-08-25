#ifndef VORONOI_PLOT_H
#define VORONOI_PLOT_H

#include "geometry.h"
#include <stdio.h>

FILE *popen_gnuplot(void);

void start_plot(FILE *pipe);
void end_plot(FILE *pipe);

void add_point(FILE *pipe, point2D_T point, const char *config);
void add_seeds(FILE *pipe, const point2D_T *seeds, int N);
void add_circle(FILE *pipe, point2D_T center, double radius);

void plot_seeds(const point2D_T *seeds, int N);

#endif
