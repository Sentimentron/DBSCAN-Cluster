#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 3, 3));
    assert(ref->root->region.width == 4);
    assert(ref->root->region.height == 4);
    assert(ref->root->region.nw.x == 0);
    assert(ref->root->region.nw.y == 0);
    assert(ref->root->region.se.x == 3);
    assert(ref->root->region.se.y == 3);

    assert(!quadtree_init(&ref, 4, 3));
    assert(ref->root->region.width == 5);
    assert(ref->root->region.height == 4);
    assert(ref->root->region.nw.x == 0);
    assert(ref->root->region.nw.y == 0);
    assert(ref->root->region.se.x == 4);
    assert(ref->root->region.se.y == 3);

    assert(!quadtree_init(&ref, 3, 4));
    assert(ref->root->region.width == 4);
    assert(ref->root->region.height == 5);
    assert(ref->root->region.nw.x == 0);
    assert(ref->root->region.nw.y == 0);
    assert(ref->root->region.se.x == 3);
    assert(ref->root->region.se.y == 4);

    return 0;
}