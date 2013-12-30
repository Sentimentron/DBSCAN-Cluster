#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y);

int quadtree_node_only_blank(QUADTREE_NODE *n) {
    char test_array[sizeof(QUADTREE_POINT) * 4];
    memset(test_array, 0xFFFFFFFF, sizeof(QUADTREE_POINT) * 4);
    return !memcmp(test_array, n->points, sizeof(QUADTREE_POINT) * 4);
}

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 3, 3));
    assert(quadtree_insert(ref, 3, 0));
    assert(quadtree_insert(ref, 2, 1));
    assert(quadtree_insert(ref, 0, 3));
    assert(quadtree_insert(ref, 1, 2));
    assert(quadtree_insert(ref, 1, 3));

    assert(quadtree_node_only_blank(ref->root->nw));
    assert(quadtree_node_only_blank(ref->root->se));

    assert(ref->root->ne->points[0].x == 3);
    assert(ref->root->ne->points[0].y == 0);
    assert(ref->root->ne->points[1].x == 2);
    assert(ref->root->ne->points[1].y == 1);
    assert(ref->root->sw->points[0].x == 0);
    assert(ref->root->sw->points[0].y == 3);
    assert(ref->root->sw->points[1].x == 1);
    assert(ref->root->sw->points[1].y == 2);

    // Check geometry
    assert(ref->root->region.nw.x == 0);
    assert(ref->root->region.nw.y == 0);
    assert(ref->root->region.se.x == 3);
    assert(ref->root->region.se.y == 3);
    assert(ref->root->region.width == 4);

    return 0;
}