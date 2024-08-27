#include "include/geometry.h"
#include "include/constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


point2D_T *random_seeds(double size, int N)
{
  point2D_T *seeds = calloc(N, sizeof(point2D_T));

  for (int ii=0; ii<N; ii++) {
    seeds[ii].x = (double)rand() / RAND_MAX * size;
    seeds[ii].y = (double)rand() / RAND_MAX * size;
  }

  return seeds;
}


void write_seeds_txt(point2D_T *seeds, int N, const char *fname)
{
  FILE *fp = fopen(fname, "w");
  for (int ii=0; ii<N; ii++) {
    fprintf(fp, "%f, %f\n", seeds[ii].x, seeds[ii].y);
  }
  fclose(fp);
}


int points_equal(point2D_T p1, point2D_T p2)
{
  return (fabs(p1.x - p2.x) < EPS && fabs(p1.y - p2.y) < EPS);
}


int points_unique_3(point2D_T p1, point2D_T p2, point2D_T p3)
{
  return (!points_equal(p1, p2) && 
          !points_equal(p2, p3) && 
          !points_equal(p1, p3));
}


double parabola(point2D_T f, double directrix, double x)
{
  return (x - f.x)*(x - f.x) / (2*(f.y - directrix)) + (f.y + directrix) / 2;
}


roots2_T intersect_parabs(point2D_T f1, point2D_T f2, double directrix)
{
  double k = directrix;
  double a = f1.x, b = f1.y, c = f2.x, d = f2.y;

  roots2_T out;
  if (d == b && d == directrix) {
    printf("ATTENTION!! Critical case for intersection... d = b\n");
    exit(1);
  } else if (a == b && c == d) { 
    printf("ATTENTION!! Same parabolas, infinite intersections\n");
    exit(1);
  } else if (d == b) {
    out.pos = (a+c)/2;
    out.neg = out.pos;
  } else {
    double A = d - b; 
    double B = 2 * (c*b - a*d + a*k - c*k);
    double C = a*a*(d-k) - c*c*(b-k) - (b-k)*(d-k)*(d-b);

    if (B*B - 4*A*C <= 0) {
      printf("ATTENTION!! NEGATIVE DISCRIMINANT......\n");
      printf("Is directrix < f1.y, f2.y??\n");
      exit(1);
    }
    out.pos = (-B - sqrt(B*B - 4*A*C)) / (2*A);
    out.neg = (-B + sqrt(B*B - 4*A*C)) / (2*A);
  }

  if (out.pos < out.neg) {
    double aux = out.neg;
    out.neg = out.pos;
    out.pos = aux;
  }
  return out;
}


circle_T points2circle(point2D_T p1, point2D_T p2, point2D_T p3)
{
  double M11 = p1.x*(p2.y - p3.y) - p2.x*(p1.y - p3.y) + p3.x*(p1.y - p2.y);
  if (M11 == 0) {
    printf("ERROR! Points are collinear. Cannot find associated circle...\n");
    exit(1);
  }
  double r1_2 = p1.x * p1.x + p1.y * p1.y;
  double r2_2 = p2.x * p2.x + p2.y * p2.y;
  double r3_2 = p3.x * p3.x + p3.y * p3.y;
  
  double M12 = r1_2*(p2.y - p3.y) - r2_2*(p1.y - p3.y) + r3_2*(p1.y - p2.y);
  double M13 = r1_2*(p2.x - p3.x) - r2_2*(p1.x - p3.x) + r3_2*(p1.x - p2.x);
  double M14 = r1_2 * (p2.x*p3.y - p2.y*p3.x) - 
               r2_2 * (p1.x*p3.y - p1.y*p3.x) + 
               r3_2 * (p1.x*p2.y - p1.y*p2.x);

  circle_T out;
  out.c.x = 0.5 * M12 / M11;
  out.c.y = - 0.5 * M13 / M11;
  out.R = sqrt(pow(out.c.x, 2) + pow(out.c.y, 2) + M14 / M11);

  return out;
}



