#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 3, 3));

    assert(quadtree_insert(ref, 3, 0));
    assert(quadtree_insert(ref, 2, 1));
    assert(quadtree_insert(ref, 0, 3));
    assert(quadtree_insert(ref, 1, 2));
    assert(quadtree_insert(ref, 1, 3));

    assert(quadtree_query(ref, 3, 0));
    assert(quadtree_query(ref, 2, 1));
    assert(quadtree_query(ref, 0, 3));
    assert(quadtree_query(ref, 1, 2));
    assert(quadtree_query(ref, 1, 3));
    assert(!quadtree_query(ref, 2, 0));

    return 0;
}