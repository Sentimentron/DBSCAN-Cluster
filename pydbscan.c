#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>
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

unsigned int *pyDBSCAN(QUADTREE *data, unsigned int dlen,
            float eps, unsigned int minpoints
    )
{
    unsigned int i;
    PyObject *ret; 
    // TODO: return a python list
    //       wrap the dlen parameter
    unsigned int *d = calloc(dlen+1, sizeof(unsigned int));
    if (d == NULL) {
        return NULL;
    }
    DBSCAN(data, d+1, dlen, eps, minpoints, &neighbours_search);
    *d = dlen;

    return d;
}
