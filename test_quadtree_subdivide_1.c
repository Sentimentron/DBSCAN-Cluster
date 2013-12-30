#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int _quadtree_node_subdivide(QUADTREE_NODE *n);

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 3, 3));
    assert(quadtree_insert(ref, 0, 0));
    assert(quadtree_insert(ref, 0, 3));
    assert(quadtree_insert(ref, 3, 0));
    assert(quadtree_insert(ref, 3, 3));

    assert(ref->root->region.width == 4);
    assert(ref->root->region.height == 4);

    assert(!_quadtree_node_subdivide(ref->root));

    // Check the geometry of each subdivision
    assert(ref->root->nw->region.nw.x == 0);
    assert(ref->root->nw->region.nw.y == 0);
    assert(ref->root->nw->region.se.x == 1); 
    assert(ref->root->nw->region.se.y == 1);
    assert(ref->root->nw->region.width == 2);
    assert(ref->root->nw->region.height == 2);

    assert(ref->root->ne->region.nw.x == 2);
    assert(ref->root->ne->region.nw.y == 0);
    assert(ref->root->ne->region.se.x == 3); 
    assert(ref->root->ne->region.se.y == 1);
    assert(ref->root->ne->region.width == 2);
    assert(ref->root->ne->region.height == 2);

    assert(ref->root->sw->region.nw.x == 0);
    assert(ref->root->sw->region.nw.y == 2);
    assert(ref->root->sw->region.se.x == 1); 
    assert(ref->root->sw->region.se.y == 3);
    assert(ref->root->sw->region.width == 2);
    assert(ref->root->sw->region.height == 2);

    assert(ref->root->se->region.nw.x == 2);
    assert(ref->root->se->region.nw.y == 2);
    assert(ref->root->se->region.se.x == 3); 
    assert(ref->root->se->region.se.y == 3);
    assert(ref->root->se->region.width == 2);
    assert(ref->root->se->region.height == 2);

    // Check the points within each subdivision
    assert(ref->root->nw->points[0].x == 0);
    assert(ref->root->nw->points[0].y == 0);

    assert(ref->root->ne->points[0].x == 3);
    assert(ref->root->ne->points[0].y == 0);

    assert(ref->root->sw->points[0].x == 0);
    assert(ref->root->sw->points[0].y == 3);

    assert(ref->root->se->points[0].x == 3);
    assert(ref->root->se->points[0].y == 3);

    return 0;
}