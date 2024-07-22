#ifndef VORONOI_FORTUNE_H
#define VORONOI_FORTUNE_H

#include "geometry.h"
#include "beachline.h"
#include "queue.h"

typedef struct vertex_T {
  double x, y;
  int ii;
  struct edge_T *inc_edge;
} vertex_T;

typedef struct edge_T {
  struct edge_T *twin;
  struct vertex_T *start;
  struct face_T *inc_face;  // Left face
  struct edge_T *next;
  struct edge_T *prev;
} edge_T;

typedef struct face_T {
  struct edge_T *edges;
  point2D_T *seed;
} face_T;

typedef struct vor_diagram_T {
  int N_faces;
  struct face_T *faces;
} vor_diagram_T;

void event_site(queue_T *queue, beachline_T *bline, event_T event);

void event_vertex(queue_T *queue, beachline_T *bline, event_T event);

vor_diagram_T *fortune_algorithm(point2D_T *seeds, int N);


#endif
