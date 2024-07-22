#include "include/beachline.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>


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


