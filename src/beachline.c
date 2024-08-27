#include "include/beachline.h"
#include <stdio.h>
#include <math.h>
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


double intersection_arcs(point2D_T f, point2D_T f_right, double directrix) {
  roots2_T intersect = intersect_parabs(f, f_right, directrix);
  double middle = (f.x + f_right.x) / 2;

  double d1 = fabs(intersect.neg - middle);
  double d2 = fabs(intersect.pos - middle);
  if (f.x < f_right.x) {
    if (d1 < d2) {
      return intersect.neg;
    } else {
      return intersect.pos;
    }
  } else {
    if (d1 < d2) {
      return intersect.pos;
    } else {
      return intersect.neg;
    }
  }

}


roots2_T arc_bounds(arc_T *arc, double directrix)
{
  roots2_T bounds = {.neg = -FLT_MAX, .pos = FLT_MAX};
  if (!arc) {
    printf("ERROR!! Trying to find bounds of invalid arc...\n");
    exit(1);
  }
  
  arc_T *left = arc->left;
  arc_T *right = arc->right;

  if (left && right) {
    bounds.neg = intersection_arcs(left->focus, arc->focus, directrix);
    bounds.pos = intersection_arcs(arc->focus, right->focus, directrix);
  } else if (!left && right) {
    bounds.pos = intersection_arcs(arc->focus, right->focus, directrix);
  } else if (left && !right) {
    bounds.neg = intersection_arcs(left->focus, arc->focus, directrix);
  }

  return bounds; 
}


arc_T *find_arc_above(beachline_T bline, point2D_T focus)
{
  // Return pointer to arc of beachline that lies directly above new focus.
  // This is done by comparing focus.x with bounds of existing arcs
  if (!bline) {
    return NULL;
  }

  arc_T *current = bline;
  while (current->right) {
    roots2_T bounds = arc_bounds(current, focus.y);
    if (focus.x > bounds.neg && focus.x < bounds.pos) {
      return current;
    }

    current = current->right;
  }
  
  return current;
}


arc_T *insert_arc(beachline_T *bline, arc_T *arc_above, point2D_T focus)
{
  arc_T *new = new_arc(focus);
  if (!arc_above) {
    *bline = new;
    return new;
  }

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


