#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadtree.h"
#include "dbscan.h" 

int nextpow2(int t) {
    int i;
    for(i=1; i < t; i*=2);
    return i;
}

QUADTREE *create_quadtree(int width, int height) {
    QUADTREE *ret = NULL;
    assert(!quadtree_init(&ret, nextpow2(width)-1, nextpow2(height)-1));
    return ret;
}

int pyDBSCAN(void *data, unsigned int *d, unsigned int dlen,
            float eps, unsigned int minpoints
    )
{
    return DBSCAN(data, d, dlen, eps, minpoints, &neighbours_search);
}
