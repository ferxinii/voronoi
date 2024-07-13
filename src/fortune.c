#include <stdlib.h>
#include <stdio.h>
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
    printf("ERROR: cannot allocate memory for event!\n");
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
    event_T *previous;
    int insert_id = 0;
    while (seeds[ii].y < current->y) {
      insert_id ++;
      previous = current;
      if (current->next) {
        current = current->next;
      } else {
        break;
      }
    }

    // Insert event in queue:
    //     .....  --> |previous|  -->  |current| -->  ......
    //     .....  --> |previous|  -->  |new|  -->  |current| -->  .....
    event_T *new = new_event(type, seeds[ii].x, seeds[ii].y);
    if (insert_id == 0) {
      new->next = current;
      head = new;
    } else if (!previous->next) {
      new->next = NULL;
      previous->next = new;
    } else {
      new->next = current;
      previous->next = new;
    }
  }

  return head;
}


void print_queue(const event_T *queue) 
{
  if (!queue) {
    printf("ERROR: trying to print invalid queue!\n");
    return;
  }
  
  const event_T *current = queue;
  int ii = 0;

  printf("%d  ", ii);
  print_event(current);
  while (current->next) {
    ii++;
    current = current->next;
    printf("%d  ", ii);
    print_event(current);
  }
  
  return;
}


void print_event(const event_T *event)
{
  if (!event) {
    printf("ERROR: trying to print invalid event!\n");
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
  printf("x: %f, y: %f\n", event->x, event->y);
}
