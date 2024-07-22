#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "include/fortune.h"


point2D_T *random_seeds(double size, int N)
{
  point2D_T *seeds = calloc(N, sizeof(point2D_T));

  for (int ii=0; ii<N; ii++) {
    seeds[ii].x = (double)rand() / RAND_MAX * size;
    seeds[ii].y = (double)rand() / RAND_MAX * size;
  }

  return seeds;
}


int points_equal(point2D_T p1, point2D_T p2)
{
  return (p1.x == p2.x && p1.y == p2.y);
}


int points_unique_3(point2D_T p1, point2D_T p2, point2D_T p3)
{
  return (!points_equal(p1, p2) && 
          !points_equal(p2, p3) && 
          !points_equal(p1, p3));
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


void print_beachline(beachline_T bline) 
{
  if (!bline) {
    printf("ERROR: trying to print invalid beachline\n");
    return;
  }

  printf("------------ BEACHLINE -----------\n");
  int ii = 0;
  printf("%d  x: %f, y: %f, p: %p,   left:%p, right:%p\n", ii,
        bline->focus.x, bline->focus.y, (void*)bline, 
        (void*)bline->left, (void*)bline->right);
  while (bline->right) {
    ii++;
    bline = bline->right;
    printf("%d  x: %f, y: %f, p: %p,   left:%p, right:%p\n", ii,
            bline->focus.x, bline->focus.y, (void*)bline, 
            (void*)bline->left, (void*)bline->right);
  }
  printf("--------------- END --------------\n");
}


arc_T *new_arc(point2D_T focus)
{
    arc_T *arc = malloc(sizeof(arc_T));
    if (!arc) {
      printf("ERROR: Cannot allocate memory for arc\n");
      exit(1);
    }
    
    arc->focus = focus;
    arc->right = NULL;
    arc->left = NULL;
    
    return arc;
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
  return out;
}


arc_T *find_arc_above(beachline_T bline, point2D_T focus)
{
  // Return pointer to arc of beachline that lies directly above new focus.
  // This is done by comparing focus.x with intersections of existing arcs
  if (!bline) {
    printf("ERROR! Trying to find arc of invalid bline\n");
    exit(1);
  }

  arc_T *current = bline;
  arc_T *next = bline->right;

  roots2_T next_intersect;

  double x_prev = - FLT_MAX;
  double x_next;
  while (next) {
    next_intersect = intersect_parabs(current->focus, 
                                          next->focus, 
                                          focus.y);
    x_next = next_intersect.neg;
    if (focus.x > x_prev && focus.x < x_next) {
      return current;
    } else if (focus.x == x_prev || focus.x == x_next) {
      printf("ERROR!! Intersection just above focus..\n");
      exit(1);
    } else {
      x_prev = x_next; 
      current = next;
      next = current->right;
    }
  }

  // REACHED RIGHTMOST ARC
  if (focus.x > x_prev) {
    return current;
  } else {
    printf("ERROR!! Reached rightmost arc without finding arc above...\n");
    exit(1);
  }
}


arc_T *insert_arc(beachline_T *bline, const point2D_T focus)
{
  // Check if beachline is empty
  if (!*bline) {
    *bline = new_arc(focus);
    return *bline;
  }
  
  arc_T *arc_above = find_arc_above(*bline, focus);
  arc_T *new = new_arc(focus);
  arc_T *above_copy = new_arc(arc_above->focus);
  
  // above  
  // above -> new -> above_copy
  above_copy->right = arc_above->right;
  above_copy->left = new;
  new->right = above_copy;
  new->left = arc_above;
  if (arc_above->right) {
    arc_above->right->left = above_copy;
  }
  arc_above->right = new;
  
  return new;
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


int circle_contains_seeds_p(queue_T queue, circle_T circle) 
{
  event_T *event = queue;
  while (event) {
    if (event->type == EVENT_SITE) {
      double d_2 = pow((event->p.x-circle.c.x), 2) + pow((event->p.y-circle.c.y), 2);
      if (d_2 <= circle.R * circle.R) {
        return 1;
      }
    }
    event = event->next;
  }

  return 0;
}


int event_exists_p(queue_T queue, event_T event)
{
  event_T *current = queue;
  while (current) {
    if (current->type == event.type &&
        points_equal(current->p, event.p) &&
        current->arc == event.arc) {
      return 1;
    }
    current = current->next;
  }

  return 0;
}


void add_event_if_nonexistent(queue_T *queue, enum event_type type, 
                              point2D_T p, arc_T *arc)
{
  if (!queue) {
    printf("ERROR!! Can't add event to invalid queue...\n");
    exit(1);
  }

  event_T *new = new_event(type, p, arc);
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


void add_vertex_events_involving(queue_T *queue, arc_T *arc)
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
        if (p.y < arc->focus.y && !circle_contains_seeds_p(*queue, circ)) {
          add_event_if_nonexistent(queue, EVENT_VERTEX, p, left); 
        }
    }
  }

  if (left && arc && right && 
      points_unique_3(left->focus, arc->focus, right->focus)) {
    circ = points2circle(left->focus, arc->focus, right->focus);
    p.x = circ.c.x;
    p.y = circ.c.y - circ.R;
    if (p.y < arc->focus.y && !circle_contains_seeds_p(*queue, circ)) {
      add_event_if_nonexistent(queue, EVENT_VERTEX, p, arc); 
    }
  }

  if (arc && right) {
    right2 = right->right;
    if (right2 && points_unique_3(arc->focus, right->focus, right2->focus)) {
      circ = points2circle(arc->focus, right->focus, right2->focus);
      p.x = circ.c.x;
      p.y = circ.c.y - circ.R;
      if (p.y < arc->focus.y && !circle_contains_seeds_p(*queue, circ)) {
        add_event_if_nonexistent(queue, EVENT_VERTEX, p, right); 
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


void delete_arc(beachline_T *bline, arc_T *arc)
{
  if (!arc || !*bline) {
    printf("ERROR! Invalid pointers in delete_arc...\n");
    exit(1);
  }
  
  // Case arc is the left-most
  if (*bline == arc) {
    *bline = arc->right;
    (*bline)->left = NULL;
    free(arc);
    return;
  }

  arc_T *right = arc->right;
  arc_T *left = arc->left;
  if (right) {
    right->left = arc->left;
  }
  // If here, arc should already have a valid left side...
  if (!left) {
    printf("Uh oh should not see this, invalid left arc in delete_arc\n");
    exit(1);
  }
  left->right = arc->right;
  free(arc);
}


void free_beachline(beachline_T bline)
{
  if (!bline) {
    return;
  }
  
  arc_T *next = bline->right;
  free(bline);
  while(next) {
    bline = next;
    next = bline->right;
    free(bline);
  }
}


vor_diagram_T *fortune_algorithm(point2D_T *seeds, int N)
{
  queue_T queue = initialize_queue(seeds, N);

  vor_diagram_T *diagram = NULL;
  beachline_T bline = NULL;

  while (queue) {
    event_T event = pop_event(&queue);
    putchar('\n');
    print_event(&event);
    print_queue(queue);
    print_beachline(bline);

    if (event.type == EVENT_SITE) {
      arc_T *arc = insert_arc(&bline, event.p);
      remove_vertex_events_involving(&queue, arc->left);
      add_vertex_events_involving(&queue, arc);
      
    } else if (event.type == EVENT_VERTEX) {
      arc_T *left = event.arc->left; 
      arc_T *right = event.arc->right;
      remove_vertex_events_involving(&queue, event.arc);
      delete_arc(&bline, event.arc);
      if (left) add_vertex_events_involving(&queue, left);
      if (right) add_vertex_events_involving(&queue, right);

    } else {
      printf("ERROR! Unknown event! Should never see this...\n");
      exit(1);
    }

 }

  free_beachline(bline);

  return diagram;
}



