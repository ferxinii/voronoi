#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "include/fortune.h"


seed_T *random_seeds(double size, int N)
{
  seed_T *seeds = calloc(N, sizeof(seed_T));

  for (int ii=0; ii<N; ii++) {
    seeds[ii].x = (double)rand() / RAND_MAX * size;
    seeds[ii].y = (double)rand() / RAND_MAX * size;
  }

  return seeds;
}


event_T *new_event(enum event_type type, double x, double y)
{
  event_T *event = malloc(sizeof(event_T));
  if (!event) {
    printf("ERROR: cannot allocate memory for event\n");
    exit(1);
  }

  event->next = NULL;
  event->type = type;
  event->x = x;
  event->y = y;
  
  return event;
}


event_T *initialize_queue(const seed_T *seeds, int N)
{
  enum event_type type = EVENT_SITE;
  
  event_T *head = new_event(type, seeds[0].x, seeds[0].y);

  for (int ii=1; ii<N; ii++) {
    // Find where to insert new event ordering by descending Y coordinate
    //          |event|  -->   |event|  -->  ......  |event|  -->  NULL
    //ID:         ----0----      ----1----            ----k-1---    k
    event_T *current = head;
    event_T *next = current->next;
    while (next && seeds[ii].y < next->y){
      current = next;
      next = current->next;
    }

    // Insert event in queue:
    //     .....  --> |current|  -->  |next| -->  ......
    //     .....  --> |current|  -->  |new|  -->  |next| -->  .....
    event_T *new = new_event(type, seeds[ii].x, seeds[ii].y);
    if (current == head && new->y > head->y) {
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


void print_queue(const event_T *queue) 
{
  if (!queue) {
    printf("ERROR: trying to print invalid queue\n");
    return;
  }
  
  const event_T *current = queue;
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
  
  return;
}


void print_event(const event_T *event)
{
  if (!event) {
    printf("ERROR: trying to print invalid event\n");
    return;
  }
  
  switch (event->type) {
    case EVENT_SITE:
      printf("SITE EVENT  ");
      break;
    case EVENT_VERTEX:
      printf("VERTEX EVENT  ");
      break;
    default:
      printf("??? (SHOULD NEVER SEE THIS)");
  }
  printf("x: %f, y: %f\n", event->x, event->y);
}


void print_beachline(const arc_T *bline) 
{
  if (!bline) {
    printf("ERROR: trying to print invalid beachline\n");
    return;
  }

  const arc_T *leftmost = bline;
  while (leftmost->left) {
    leftmost = leftmost->left;
  }
 
  printf("------------ BEACHLINE -----------\n");
  int ii = 0;
  printf("%d  x: %f, y: %f\n", ii, leftmost->focus.x, leftmost->focus.y);
  while (leftmost->right) {
    ii++;
    leftmost = leftmost->right;
    printf("%d  x: %f, y: %f\n", ii, leftmost->focus.x, leftmost->focus.y);
  }
  printf("--------------- END --------------\n");
  return;
}


arc_T *new_arc(focus_T focus)
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


double parabola_y(focus_T f, double directrix, double x)
{
  return (x - f.x)*(x - f.x) / (2*(f.y - directrix)) + (f.y + directrix) / 2;
}


parab_intersect_T x_intersection(focus_T f1, focus_T f2, double directrix)
{
  double k = directrix;
  double a = f1.x;
  double b = f1.y;
  double c = f2.x;
  double d = f2.y;

  double A, B, C;
  parab_intersect_T out;

  if (d == b && d == directrix) {
    printf("ATTENTION!! Critical case for intersection... d = b\n");
    exit(1);
  } else if (a == b && c == d) { 
    printf("ATTENTION!! Same parabolas, infinite intersections\n");
    exit(1);
  } else if (d == b) {
    //B = 2 * (c*b - a*d + a*k - c*k);
    //C = a*a*(d-k) - c*c*(b-k);
    // Actually, it simplifies further...
    out.x_left = (a+c)/2;
    out.x_right = out.x_left;
  } else {
    A = d - b; 
    B = 2 * (c*b - a*d + a*k - c*k);
    C = a*a*(d-k) - c*c*(b-k) - (b-k)*(d-k)*(d-b);
    
    if (B*B - 4*A*C <= 0) {
      printf("ATTENTION!! NEGATIVE DISCRIMINANT......\n");
      printf("Is directrix < f1.y, f2.y??\n");
      exit(1);
    }
    
    out.x_left = (-B - sqrt(B*B - 4*A*C)) / (2*A);
    out.x_right = (-B + sqrt(B*B - 4*A*C)) / (2*A);
  }
  //printf("%f, %f\n", out.x_left, out.x_right);
  return out;
}


arc_T *find_arc_above(arc_T *bline, const focus_T focus)
{
  // Return pointer to arc of beachline that lies directly above new focus.
  // This is done by comparing focus.x with intersections of existing arcs
  if (!bline) {
    printf("ERROR! Trying to find arc of invalid bline\n");
    exit(1);
  }

  double sweepline_y = focus.y;
  parab_intersect_T cross_left, cross_right;
  
  arc_T *left = bline->left;
  arc_T *right = bline->right;

  while (1) {
    if (!left && !right) {
      // ONLY ONE ARC
      return bline;
    } else if (!left && right) {
      // ONLY 2 ARCS: BLINE AND RIGHT
      cross_right = x_intersection(bline->focus, 
                                   right->focus, 
                                   sweepline_y);
      if (focus.x < cross_right.x_left) {
        return bline;
      } else if (focus.x == cross_right.x_left) {
        printf("ERROR!! Intersection just above focus..\n");
        exit(1);
      } else {
        bline = bline->right;
        right = bline->right;
        left = bline->left;
      }
    } else if (left && !right) {
      // ONLY 2 ARCS: BLINE AND LEFT
      cross_left = x_intersection(bline->focus, 
                                   left->focus, 
                                   sweepline_y);
      if (focus.x > cross_left.x_right) {
        return bline;
      } else if (focus.x == cross_left.x_right) {
        printf("ERROR!! Intersection just above focus..\n");
        exit(1);
      } else {
        bline = bline->left;
        right = bline->right;
        left = bline->left;
      }
    } else {
      cross_left = x_intersection(bline->focus, 
                                   left->focus, 
                                   sweepline_y);
      cross_right = x_intersection(bline->focus, 
                                   right->focus, 
                                   sweepline_y);
      if (focus.x > cross_right.x_left) {
        bline = right;
        right = bline->right;
        left = bline->left;
      } else if (focus.x < cross_left.x_right) {
        bline = left;
        left = bline->left;
        right = bline->right;
      } else if (focus.x > cross_left.x_right && 
                 focus.x < cross_right.x_left) {
        return bline;
      } else if (focus.x == cross_left.x_right || 
                 focus.x == cross_left.x_right) {
        printf("ERROR!! Intersection just above focus..\n");
        exit(1);
      }
    }
  }
}


arc_T *insert_arc(arc_T **bline, const focus_T focus)
{
  // Check if beachline is empty
  if (!*bline) {
    *bline = new_arc(focus);
    return *bline;
  }
  
  arc_T *arc_above = find_arc_above(*bline, focus);
  arc_T *new = new_arc(focus);
  arc_T *above_copy = new_arc(arc_above->focus);
  
  above_copy->right = arc_above->right;
  above_copy->left = new;
  new->right = above_copy;
  new->left = arc_above;
  arc_above->right = new;

  return new;
}


vor_diagram_T *fortune_algorithm(seed_T *seeds, int N)
{
  event_T *queue = initialize_queue(seeds, N);
  print_queue(queue);

  vor_diagram_T *diagram = NULL;
  arc_T *bline = NULL;

  while (queue) {
    event_T event = pop_event(&queue);

    if (event.type == EVENT_SITE) {
      // Add arc into beachline
      focus_T focus = {.x = event.x, .y = event.y};
      arc_T *arc = insert_arc(&bline, focus);
      print_beachline(bline); 
      putchar('\n');
      
    } else if (event.type == EVENT_VERTEX) {
      // Remove squeezed cell from the beachline

    } else {
      printf("ERROR! Unknown event! Should never see this...\n");
      exit(1);
    }

    putchar('\n');
    //print_queue(queue);
  }

  free(bline);

  return diagram;
}



