#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int _quadtree_node_subdivide(QUADTREE_NODE *n);

int main(int argc, char **argv) {
    QUADTREE *ref = NULL;

    assert(!quadtree_init(&ref, 7, 7));
    for (int i = 0; i < 8; i++) {
        assert(quadtree_insert(ref, i, i));
    }

    for (int i = 0; i < 4; i++) {
        assert(ref->root->points[i].x == i);
        assert(ref->root->points[i].x == i);
    }

    for (int i = 4; i < 8; i++) {
        assert(ref->root->se->points[i-4].x == i);
        assert(ref->root->se->points[i-4].y == i);
    }

    return 0;
}
