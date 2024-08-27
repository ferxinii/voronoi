#ifndef VORONOI_BEACHLINE_H
#define VORONOI_BEACHLINE_H

#include "geometry.h"
#include "dcel.h"

typedef struct arc_T {
  point2D_T focus;
  struct arc_T *left;
  struct arc_T *right;
  site_T *site;
  edge_T *l_edge;
  edge_T *r_edge;
} arc_T;

typedef arc_T *beachline_T;

void print_beachline(beachline_T bline);

arc_T *new_arc(point2D_T focus);

void free_beachline(beachline_T bline);

roots2_T arc_bounds(arc_T *arc, double directrix);

arc_T *find_arc_above(beachline_T bline, point2D_T focus);

arc_T *insert_arc(beachline_T *bline, arc_T *arc_above, point2D_T focus);

void delete_arc(beachline_T *bline, arc_T *arc);


#endif
