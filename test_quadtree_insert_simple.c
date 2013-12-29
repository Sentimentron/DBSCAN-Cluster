#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"


int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y);

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 2, 2));
    assert(quadtree_insert(ref, 0, 0));
    assert(quadtree_insert(ref, 0, 1));
    assert(quadtree_insert(ref, 1, 0));
    assert(quadtree_insert(ref, 1, 1));

    assert(ref->root->points[0].x == 0);
    assert(ref->root->points[1].x == 0);
    assert(ref->root->points[2].x == 1);
    assert(ref->root->points[3].x == 1);

    assert(ref->root->points[0].y == 0);
    assert(ref->root->points[1].y == 1);
    assert(ref->root->points[2].y == 0);
    assert(ref->root->points[3].y == 1);

    return 0;
}