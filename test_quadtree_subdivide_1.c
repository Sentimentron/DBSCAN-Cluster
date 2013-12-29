#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int _quadtree_node_subdivide(QUADTREE_NODE *n);

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 2, 2));
    assert(quadtree_insert(ref, 0, 0));
    assert(quadtree_insert(ref, 0, 1));
    assert(quadtree_insert(ref, 1, 0));
    assert(quadtree_insert(ref, 1, 1));

    assert(!_quadtree_node_subdivide(ref->root));

    // Check the geometry of each subdivision
    assert(ref->root->nw->region.nw.x == 0);
    assert(ref->root->nw->region.nw.y == 0);
    assert(ref->root->nw->region.se.x == 0); 
    assert(ref->root->nw->region.se.y == 0);
    assert(ref->root->nw->region.width == 1);
    assert(ref->root->nw->region.height == 1);

    assert(ref->root->ne->region.nw.x == 1);
    assert(ref->root->ne->region.nw.y == 0);
    assert(ref->root->ne->region.se.x == 1); 
    assert(ref->root->ne->region.se.y == 0);
    assert(ref->root->ne->region.width == 1);
    assert(ref->root->ne->region.height == 1);

    assert(ref->root->sw->region.nw.x == 0);
    assert(ref->root->sw->region.nw.y == 1);
    assert(ref->root->sw->region.se.x == 0); 
    assert(ref->root->sw->region.se.y == 1);
    assert(ref->root->sw->region.width == 1);
    assert(ref->root->sw->region.height == 1);

    assert(ref->root->ne->region.nw.x == 1);
    assert(ref->root->ne->region.nw.y == 1);
    assert(ref->root->ne->region.se.x == 1); 
    assert(ref->root->ne->region.se.y == 1);
    assert(ref->root->ne->region.width == 1);
    assert(ref->root->ne->region.height == 1);

    // Check the points within each subdivision
    assert(ref->root->nw->points[0].x == 0);
    assert(ref->root->nw->points[0].y == 0);

    assert(ref->root->ne->points[0].x == 1);
    assert(ref->root->ne->points[0].y == 0);

    assert(ref->root->sw->points[0].x == 0);
    assert(ref->root->sw->points[0].y == 1);

    assert(ref->root->se->points[0].x == 1);
    assert(ref->root->se->points[0].y == 1);

    return 0;
}