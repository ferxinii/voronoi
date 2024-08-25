#ifndef VORONOI_DCEL_H
#define VORONOI_DCEL_H

#include "geometry.h"

typedef struct vertex_T {
  double x, y;
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

typedef struct site_T {
  point2D_T seed;
  face_T *face;
} site_T;


site_T *initialise_sites(point2D_T *seeds, int N);

edge_T *malloc_edge(void);

vertex_T *malloc_vertex(void);


#endif
