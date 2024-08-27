#include "include/queue.h"
#include "include/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_queue(queue_T queue) 
{
  if (!queue) {
    printf("ERROR: trying to print invalid queue\n");
    return;
  }
  
  event_T *current = queue;
  int ii = 0;
  
  printf("-------------- QUEUE -------------\n");
  printf("%d  ", ii);
  print_event(current);
  while (current->next) {
    ii++;
    current = current->next;
    printf("%d  ", ii);
    print_event(current);
  }
  printf("--------------- END --------------\n");
}


void print_event(event_T *event)
{
  if (!event) {
    printf("ERROR: trying to print invalid event\n");
    return;
  }
  
  switch (event->type) {
    case EVENT_SITE:
      printf("SITE EVENT    ");
      break;
    case EVENT_VERTEX:
      printf("VERTEX EVENT  ");
      break;
    default:
      printf("??? (SHOULD NEVER SEE THIS)");
  }
  printf("x: %f, y: %f", event->p.x, event->p.y);
  if (event->arc) {
    printf("    ARC: %f, %f, %p", event->arc->focus.x, event->arc->focus.y,
          (void*)event->arc);
  }
  putchar('\n');
}


event_T *new_event(enum event_type type, point2D_T p, arc_T *arc)
{
  event_T *event = malloc(sizeof(event_T));
  if (!event) {
    printf("ERROR: cannot allocate memory for event\n");
    exit(1);
  }
  
  event->next = NULL;
  event->type = type;
  event->p = p;
  event->arc = arc;
  
  return event;
}


queue_T initialize_queue(const point2D_T *seeds, int N)
{
  enum event_type type = EVENT_SITE;
  
  event_T *head = new_event(type, seeds[0], NULL);

  for (int ii=1; ii<N; ii++) {
    // Find where to insert new event ordering by descending Y coordinate
    //          |event|  -->   |event|  -->  ......  |event|  -->  NULL
    //ID:         ----0----      ----1----            ----k-1---    k
    event_T *current = head;
    event_T *next = current->next;
    while (next && seeds[ii].y < next->p.y){
      current = next;
      next = current->next;
    }

    // Insert event in queue:
    //     .....  --> |current|  -->  |next| -->  ......
    //     .....  --> |current|  -->  |new|  -->  |next| -->  .....
    event_T *new = new_event(type, seeds[ii], NULL);
    if (current == head && new->p.y > head->p.y) {
      new->next = current;
      head = new;
    } else {
      new->next = next;
      current->next = new;
    }
  }

  return head;
}


event_T pop_event(event_T **queue) 
{
  event_T out = **queue;
  event_T *head = *queue;
  *queue = (*queue)->next;
  free(head);
  return out;
}


int event_exists_p(queue_T queue, event_T event)
{
  event_T *current = queue;
  while (current) {
    if (current->type == event.type &&
        points_equal(current->p, event.p)) { //&&
        //current->arc == event.arc) {
      return 1;
    }
    current = current->next;
  }

  return 0;
}


void add_event_if_nonexistent(queue_T *queue, enum event_type type, 
                              point2D_T p, point2D_T circ_c, arc_T *arc)
{
  if (!queue) {
    printf("ERROR!! Can't add event to invalid queue...\n");
    exit(1);
  }

  event_T *new = new_event(type, p, arc);
  new->circ_c = circ_c;
  // Case empty queue
  if (!*queue) {
    *queue = new;
    return;
  }
  
  // Check existance of event
  if (event_exists_p(*queue, *new)) {
    free(new);
    return;
  }
  
  event_T *current = *queue;
  event_T *next = current->next;
  while (next && next->p.y > p.y) {
    current = next;
    next = current->next;
  }
  
  if (current == *queue && p.y > current->p.y) {
    new->next = current;
    *queue = new;
  } else {
    new->next = next;
    current->next = new;
  }
}


void add_vertex_events_involving(queue_T *queue, arc_T *arc, double current_y, point2D_T *seeds, int N)
{
  arc_T *left2;
  arc_T *left = arc->left;
  arc_T *right = arc->right;
  arc_T *right2;
  
  circle_T circ;
  point2D_T p;
  if (left && arc) {
    left2 = left->left;
      if (left2 && points_unique_3(left2->focus, left->focus, arc->focus)) {
        circ = points2circle(left2->focus, left->focus, arc->focus);
        p.x = circ.c.x;
        p.y = circ.c.y - circ.R;
        if (p.y - current_y < -EPS && !circle_contains_seeds(circ, seeds, N)) {
          add_event_if_nonexistent(queue, EVENT_VERTEX, p, circ.c, left); 
          //printf("%e\n", p.y-current_y);
        }
    }
  }

  if (left && arc && right && 
      points_unique_3(left->focus, arc->focus, right->focus)) {
    circ = points2circle(left->focus, arc->focus, right->focus);
    p.x = circ.c.x;
    p.y = circ.c.y - circ.R;
    if (p.y - current_y < -EPS && !circle_contains_seeds(circ, seeds, N)) {
      add_event_if_nonexistent(queue, EVENT_VERTEX, p, circ.c, arc); 
      //printf("%e\n", p.y-current_y);
    }
  }

  if (arc && right) {
    right2 = right->right;
    if (right2 && points_unique_3(arc->focus, right->focus, right2->focus)) {
      circ = points2circle(arc->focus, right->focus, right2->focus);
      p.x = circ.c.x;
      p.y = circ.c.y - circ.R;
      if (p.y - current_y < -EPS && !circle_contains_seeds(circ, seeds, N)) {
        add_event_if_nonexistent(queue, EVENT_VERTEX, p, circ.c, right); 
        //printf("%e\n", p.y-current_y);
      }
    }
  }
}


void remove_vertex_events_involving(queue_T *queue, arc_T *arc)
{
  if (!queue) {
    printf("ERROR! Cannot remove events from invalid queue...\n");
    exit(1);
  }
  
  event_T *current = *queue;
  if (!current) {
    return;
  }
  
  // Crop head of the queue
  while (current && current->type == EVENT_VERTEX && (
        current->arc == arc ||
        current->arc->right == arc ||
        current->arc->left == arc) ) {
    *queue = current->next;
    free(current);
    current = *queue;
  }
  if (!current) {
    return;
  }
  
  event_T *next = current->next;
  while (next) {
    if (next->type == EVENT_VERTEX && (
        next->arc == arc || 
        next->arc->right == arc || 
        next->arc->left == arc) ) {
      // Delete event (Don't change current)
      current->next = next->next;
      free(next);
      next = current->next;
    } else {
      current = next;
      next = current->next;
    }
  }
}


int circle_contains_seeds(circle_T circle, point2D_T *seeds, int N) 
{
  for (int ii=0; ii<N; ii++) {
    double d_2 = pow((seeds[ii].x-circle.c.x), 2) + 
                 pow((seeds[ii].y-circle.c.y), 2);
    if (d_2 < circle.R * circle.R) {
        return 1;
    }
  }

  return 0;
}


