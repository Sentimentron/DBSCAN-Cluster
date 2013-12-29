#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quadtree.h"

int _quadtree_node_contains(QUADTREE_NODE *n, unsigned int x, unsigned int y);

int main(int argc, char **argv) {
    QUADTREE *ref = NULL; 

    assert(!quadtree_init(&ref, 4, 4));

    assert(!_quadtree_node_contains(ref->root, 0, 0));
    assert(!_quadtree_node_contains(ref->root, 0, 1));
    assert(!_quadtree_node_contains(ref->root, 0, 2));
    assert(!_quadtree_node_contains(ref->root, 0, 3));
    assert(!_quadtree_node_contains(ref->root, 0, 4));

    assert(!_quadtree_node_contains(ref->root, 1, 0));
    assert(_quadtree_node_contains(ref->root, 1, 1));
    assert(_quadtree_node_contains(ref->root, 1, 2));
    assert(_quadtree_node_contains(ref->root, 1, 3));
    assert(_quadtree_node_contains(ref->root, 1, 4));   
    
    assert(!_quadtree_node_contains(ref->root, 2, 0));
    assert(_quadtree_node_contains(ref->root, 2, 1));
    assert(_quadtree_node_contains(ref->root, 2, 2));
    assert(_quadtree_node_contains(ref->root, 2, 3));
    assert(_quadtree_node_contains(ref->root, 2, 4));

    assert(!_quadtree_node_contains(ref->root, 3, 0));
    assert(_quadtree_node_contains(ref->root, 3, 1));
    assert(_quadtree_node_contains(ref->root, 3, 2));
    assert(_quadtree_node_contains(ref->root, 3, 3));
    assert(_quadtree_node_contains(ref->root, 3, 4));

    assert(!_quadtree_node_contains(ref->root, 4, 0));
    assert(_quadtree_node_contains(ref->root, 4, 1));
    assert(_quadtree_node_contains(ref->root, 4, 2));
    assert(_quadtree_node_contains(ref->root, 4, 3));
    assert(_quadtree_node_contains(ref->root, 4, 4));

    assert(!_quadtree_node_contains(ref->root, 5, 0));
    assert(!_quadtree_node_contains(ref->root, 5, 1));
    assert(!_quadtree_node_contains(ref->root, 5, 2));
    assert(!_quadtree_node_contains(ref->root, 5, 3));
    assert(!_quadtree_node_contains(ref->root, 5, 4));

    return 0;
}