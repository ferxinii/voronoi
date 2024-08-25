#ifndef VORONOI_GEOMETRY_H
#define VORONOI_GEOMETRY_H

// ---- Points ----
typedef struct point2D_T {
  double x;
  double y;
} point2D_T;

point2D_T *random_seeds(double size, int N);

void write_seeds_txt(point2D_T *seeds, int N, const char *fname);

int points_equal(point2D_T p1, point2D_T p2);

int points_unique_3(point2D_T p1, point2D_T p2, point2D_T p3);


// ---- Parabolas and geometry ----

double parabola(point2D_T f, double directrix, double x);

typedef struct roots2_T {
  double pos;
  double neg;
} roots2_T;

roots2_T intersect_parabs(point2D_T f1, point2D_T f2, double directrix_y);

typedef struct circle_T {
  point2D_T c;
  double R;
} circle_T;

circle_T points2circle(point2D_T p1, point2D_T p2, point2D_T p3);

#endif
