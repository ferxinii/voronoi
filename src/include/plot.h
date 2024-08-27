#ifndef VORONOI_PLOT_H
#define VORONOI_PLOT_H

#include "geometry.h"
#include "beachline.h"
#include <stdio.h>

void remove_files_in_directory(const char *path);

FILE *popen_gnuplot(char *file_output);

void start_plot(FILE *pipe);
void end_plot(FILE *pipe);

void add_point(FILE *pipe, point2D_T point, const char *config);
void add_seeds(FILE *pipe, const point2D_T *seeds, int N);
void add_circle(FILE *pipe, point2D_T center, double radius);

void plot_seeds(const point2D_T *seeds, int N);

void add_parabola(FILE *pipe, point2D_T focus, double directrix, double min_x, double max_x);
void add_yline(FILE *pipe, double y);

void add_bline(FILE *pipe, beachline_T bline, double sweep_y);

#endif
