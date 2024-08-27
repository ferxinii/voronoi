#ifndef VORONOI_QUEUE_H
#define VORONOI_QUEUE_H

#include "geometry.h"
#include "beachline.h"

enum event_type {
    EVENT_SITE,
    EVENT_VERTEX
};

typedef struct event_T {
  enum event_type type;
  struct event_T *next;
  point2D_T p; 
  point2D_T circ_c;  // Only for vertex events
  arc_T *arc;  // Only for vertex events
} event_T;

typedef event_T *queue_T;

void print_queue(queue_T queue);

void print_event(queue_T event);

event_T *new_event(enum event_type type, point2D_T p, arc_T *arc);

queue_T initialise_queue(const point2D_T *seeds, int N);

event_T pop_event(queue_T *queue);

int event_exists_p(queue_T queue, event_T event);

void add_event_if_nonexistent(queue_T *queue, enum event_type type, 
                              point2D_T p, point2D_T circ_c, arc_T *arc);

void add_vertex_events_involving(queue_T *queue, arc_T *arc, double current_y, point2D_T *seeds, int N);

void remove_vertex_events_involving(queue_T *queue, arc_T *arc);

int circle_contains_seeds(circle_T circle, point2D_T *seeds, int N);

#endif
