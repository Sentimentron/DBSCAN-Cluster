%module pydbscan
%{
#include "quadtree.h"
#include "dbscan.h"
%}

extern int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y);
extern QUADTREE *create_quadtree(int width, int height);

%typemap(out) unsigned int *pyDBSCAN %{
  $result = PyList_New($1[0]);
  for (int i = 0; i < $1[0]; ++i) {
    PyList_SetItem($result, i, PyInt_FromLong($1[i+1]));
  }
  free($1);
%}

extern unsigned int *pyDBSCAN(void *data, unsigned int dlen,
  float eps, unsigned int minpoints
);

