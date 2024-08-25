#include "include/dcel.h"
#include "include/geometry.h"
#include <stdlib.h>


site_T *initialise_sites(point2D_T *seeds, int N)
{
  site_T *sites = calloc(N, sizeof(site_T));
  for (int ii=0; ii<N; ii++) {
    sites[ii].seed = seeds[ii];
    sites[ii].face = malloc(sizeof(face_T));
  }

  return sites;
}
