#include "include/plot.h"
#include "include/geometry.h"
#include "include/beachline.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <dirent.h>
#include <string.h>

void remove_files_in_directory(const char *path)
{
  struct dirent *entry;
  DIR *dp = opendir(path);

  if (dp == NULL) {
    perror("opendir");
    return;
  }

  while ((entry = readdir(dp))) {
    // Skip the "." and ".." directories
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // Construct the full path of the file to be deleted
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

    // Remove the file
    if (remove(filepath) != 0) {
      printf("Coultd not remove file...\n");
      exit(1);
    }
  }

  closedir(dp);
}


FILE *popen_gnuplot(char *file_output)
{
  FILE *pipe = popen("gnuplot -persistent", "w");
  if (!pipe) {
    printf("ERROR! Cannot open gnuplot pipe...\n");
    exit(1);
  }

  fprintf(pipe, "set terminal png enhanced font 'Arial,18' size 1080,1080 enhanced \n");
  fprintf(pipe, "set output '%s'\n", file_output);
  fprintf(pipe, "set xrange [0:1]\n");
  fprintf(pipe, "set yrange [0:1]\n");
  fprintf(pipe, "set size square\n");

  return pipe;
}


void start_plot(FILE *pipe)
{
  fprintf(pipe, "");
  fflush(pipe);
  fprintf(pipe, "plot ");
}


void end_plot(FILE *pipe)
{
  fprintf(pipe, "\n");
  pclose(pipe);
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
  FILE *pipe = popen_gnuplot("./plot.png");
  start_plot(pipe);
  add_seeds(pipe, seeds, N);
  end_plot(pipe);
}


void add_parabola(FILE *pipe, point2D_T focus, double directrix, double min_x, double max_x)
{
  int N = 100;

  fprintf(pipe,"\"<echo \'");
  for (int ii=0; ii<N; ii++) {
    double x = min_x + ii * (max_x - min_x) / (N - 1);
    fprintf(pipe, "%f %f", x, parabola(focus, directrix, x));
    if (ii < N-1) {
      fprintf(pipe, "\\n");
    }
  }
  fprintf(pipe, "\'\" w lines notitle lt 6 lw 3, ");
}


void add_yline(FILE *pipe, double y) 
{
  fprintf(pipe, "\"<echo \'%f %f\\n%f %f'\" w line lt 7 lw 3 notitle, ", 0.0, y, 1.0, y);
}


void plot_current_frame(beachline_T bline, double sweep_y, point2D_T *seeds, int N)
{
  char filename[50];
  static int num_frame = 0;
  FILE *pipe = NULL;
  if (bline) {
    num_frame++;
    snprintf(filename, 50, "./frames/frame_%d.png", num_frame);
    //printf("%s\n", filename);
    pipe = popen_gnuplot(filename);
    start_plot(pipe);
    add_seeds(pipe, seeds, N);
    add_yline(pipe, sweep_y);
  }

  arc_T *current = bline;
  while (current) {
    roots2_T bounds = arc_bounds(current, sweep_y);
    if (bounds.neg == -FLT_MAX && bounds.pos > 0) {
      bounds.neg = 0;
    }
    if (bounds.pos == FLT_MAX && bounds.neg < 1) {
      bounds.pos = 1;
    }
    //bounds.neg = 0;
    //bounds.pos = 1;
    add_parabola(pipe, current->focus, sweep_y, bounds.neg, bounds.pos);
    current = current->right;
  }
  
  if (bline) end_plot(pipe);
}
