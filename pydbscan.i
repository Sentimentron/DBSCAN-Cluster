%module pydbscan
%{
#include "quadtree.h"
#include "dbscan.h"
%}

extern int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y);
extern QUADTREE *create_quadtree(int width, int height);

extern int pydbscan(void *data, unsigned int *d, unsigned int dlen,
  float eps, unsigned int minpoints
);
