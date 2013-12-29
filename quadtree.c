#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quadtree.h"

#define NULLCHECK(ptr) if (ptr == NULL) { \
    fprintf(stderr, "%s:%s: %s", \
        __FILE__, __LINE__, \
        "NULL pointer cannot be passed here."); \
    exit(2); \
    }

#define SAFE_MALLOC(nmemb, size, ref) *ref = calloc(nmemb, size); \
    if (*ref == NULL) { \
        fprintf(stderr, "%s:%s: %s", \
            __FILE__, __LINE__, \
            "NULL pointer cannot be passed here."); \
        exit(2); \
    } 

int _quadtree_insert(QUADTREE_NODE *n, unsigned int x, unsigned int y);

//
// Allocation functions
int _qp_alloc(QUADTREE_POINT **ref) {
    // Allocate a QUADTREE_POINT and return 0
    NULLCHECK(ref);
    SAFE_MALLOC(sizeof(QUADTREE_POINT), 1, ref);
    return 0;
}

int _qn_alloc(QUADTREE_NODE **ref) {
    // Allocate a QUADTREE_NODE and return 0
    NULLCHECK(ref);
    SAFE_MALLOC(sizeof(QUADTREE_NODE), 1, ref);
    return 0;
}

//
// Constructor functions 
int quadtree_alloc(QUADTREE **ref) {
    NULLCHECK(ref);
    SAFE_MALLOC(sizeof(QUADTREE), 1, ref);
    return 0;
}

int quadtree_init(QUADTREE **ref, unsigned int xmax, unsigned int ymax) {
    // Unit test: test_quadtree_init.c 
    NULLCHECK(ref);
    // Allocate root structure
    if (quadtree_alloc(ref)) {
        return 1;
    }
    // Allocate initial leaf 
    if (_qn_alloc(&((*ref)->root))) {
        return 1;
    }
    (*ref)->root->region.width = xmax; 
    (*ref)->root->region.height = ymax; 
    (*ref)->root->region.se.x = xmax;
    (*ref)->root->region.se.y = ymax; 
    return 0;
}

int _quadtree_node_contains(QUADTREE_NODE *n, unsigned int x, unsigned int y) {
    unsigned int minx, miny, maxx, maxy; 
    minx = n->region.nw.x; 
    miny = n->region.nw.y; 
    maxx = n->region.se.x;
    maxy = n->region.se.y; 
    return ((x >= minx) && (x <= maxx)) && ((y >= miny) && (y <= maxy));
}

int _quadtree_node_init(QUADTREE_NODE **node, unsigned int minx, unsigned int miny,
                        unsigned int maxx, unsigned int maxy) {
    if (_qn_alloc(node)) {
        return 1; 
    }

    (*node)->region.nw.x = minx;
    (*node)->region.nw.y = miny; 
    (*node)->region.se.x = maxx;
    (*node)->region.se.y = maxy;
    (*node)->region.width = maxx - minx;
    (*node)->region.height = maxy - miny;
    return 0;
}

int _quadtree_node_subdivide(QUADTREE_NODE *n) {
    QUADTREE_NODE *nw, *ne, *sw, *se; 
    unsigned int x, y, hw, hh, i;

    // Generate the new boundaries
    x = n->region.nw.x;
    y = n->region.nw.y;
    hw = n->region.width / 2;
    hh = n->region.height / 2;

    // Create the new regions 
    if (_quadtree_node_init(&nw, x,      y - hh,     x + hw,     y))      return 1;
    if (_quadtree_node_init(&ne, x + hw, y - hh,     x + hw * 2, y))      return 1;
    if (_quadtree_node_init(&sw, x,      y - hh * 2, x + hw,     y - hh)) return 1;
    if (_quadtree_node_init(&se, x + hw, y - hh * 2, x + hw * 2, y - hh)) return 1; 

    // Assign the new nodes 
    n->nw = nw; 
    n->ne = ne; 
    n->sw = sw;
    n->se = se; 

    for (i = 0; i < 4; i++) {
        if(!_quadtree_insert(n, n->points[i].x, n->points[i].y)) {
            return 1;
        }
    }

    return 0;
}

int _quadtree_insert(QUADTREE_NODE *n, unsigned int x, unsigned int y) {
    
    int i;
    char node_test[sizeof(QUADTREE_POINT)];

    // If this point is not within the boundary of this node, it can't be added
    if (!_quadtree_node_contains(n, x, y)) return 0;

    // If there is space in this node, add the object here 
    memset(node_test, sizeof node_test, 0);
    for (i = 0; i < 4; i++) {
        if (memcmp(node_test, n->points + i, sizeof(QUADTREE_POINT))) {
            continue;
        }
        n->points[i] = (QUADTREE_POINT) {x, y};
        return 1;
    }
    // Otherwise, subdivide the tree...
    if (_quadtree_node_subdivide(n)) {
        fprintf(stderr, "Subdivision error!\n");
        return 0;
    }

    // And add it to the right node 
    if (!_quadtree_insert(n->nw, x, y)) return 1;
    if (!_quadtree_insert(n->ne, x, y)) return 1;
    if (!_quadtree_insert(n->sw, x, y)) return 1;
    if (!_quadtree_insert(n->se, x, y)) return 1;

    return 0;
}

int _quadtree_node_isleaf(QUADTREE_NODE *n) {
    int ret = 1;
    ret &= (n->nw == NULL);
    ret &= (n->ne == NULL);
    ret &= (n->sw == NULL);
    ret &= (n->se == NULL); 
    return ret; 
}

int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y) {
    return _quadtree_insert(tree->root, x, y);
}

int _quadtree_query(QUADTREE_NODE *n, unsigned int x, unsigned int y) {
    int i;
    QUADTREE_NODE *c;
    // If we're at a leaf, no child nodes to search 
    if (_quadtree_node_isleaf(n)) {
        for (i = 0; i < 4; i++) {
            if (n->points[i].x == x) {
                if (n->points[i].y == y) {
                    return 1;
                }
            }
        }
        return 0;
    }

    if (_quadtree_node_contains(n->ne, x, y)) c = n->ne;
    if (_quadtree_node_contains(n->nw, x, y)) c = n->nw;
    if (_quadtree_node_contains(n->se, x, y)) c = n->se;
    if (_quadtree_node_contains(n->sw, x, y)) c = n->sw; 
    return _quadtree_query(c, x, y);
}

int quadtree_query(QUADTREE *tree, unsigned int x, unsigned int y) {

}