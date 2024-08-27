#ifndef VORONOI_FORTUNE_H
#define VORONOI_FORTUNE_H

#include "geometry.h"
#include "beachline.h"
#include "queue.h"
#include "dcel.h"

void remove_files_in_directory(const char *path);

void event_site(queue_T *queue, beachline_T *bline, event_T event);

void event_vertex(queue_T *queue, beachline_T *bline, event_T event);

site_T *fortune_algorithm(point2D_T *seeds, int N);


#endif
