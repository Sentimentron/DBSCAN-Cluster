#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quadtree.h"

#define NULLCHECK(ptr) if (ptr == NULL) { \
    fprintf(stderr, "%s:%d: %s", \
        __FILE__, __LINE__, \
        "NULL pointer cannot be passed here."); \
    exit(2); \
    }

#define SAFE_MALLOC(nmemb, size, ref) *ref = calloc(nmemb, size); \
    if (*ref == NULL) { \
        fprintf(stderr, "%s:%d: %s", \
            __FILE__, __LINE__, \
            "NULL pointer cannot be passed here."); \
        exit(2); \
    }

int _quadtree_insert(QUADTREE_NODE *n, unsigned int x, unsigned int y, int subdivide);

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

int _quadtree_node_init(QUADTREE_NODE **node, unsigned int minx, unsigned int miny,
                        unsigned int maxx, unsigned int maxy) {
    // Unit test: covered by test_quadtree_init.c
    if (_qn_alloc(node)) {
        return 1;
    }

    (*node)->region.nw.x = minx;
    (*node)->region.nw.y = miny;
    (*node)->region.se.x = maxx;
    (*node)->region.se.y = maxy;
    (*node)->region.width = maxx - minx + 1;
    (*node)->region.height = maxy - miny + 1;

    memset((*node)->points, 0xFFFFFFFF, sizeof(QUADTREE_POINT) * 4);

    return 0;
}

int quadtree_init(QUADTREE **ref, unsigned int xmax, unsigned int ymax) {
    // Unit test: test_quadtree_init.c
    NULLCHECK(ref);
    // Allocate root structure
    if (quadtree_alloc(ref)) {
        return 1;
    }

    return _quadtree_node_init(&((*ref)->root), 0, 0, xmax, ymax);
}

int _quadtree_node_contains(QUADTREE_NODE *n, unsigned int x, unsigned int y) {
    // Unit test: test_quadtree_node_contains.c
    unsigned int minx, miny, maxx, maxy;
    minx = n->region.nw.x;
    miny = n->region.nw.y;
    maxx = n->region.se.x;
    maxy = n->region.se.y;
    return ((x >= minx) && (x <= maxx)) && ((y >= miny) && (y <= maxy));
}

int _quadtree_node_subdivide(QUADTREE_NODE *n) {
    QUADTREE_NODE *nw, *ne, *sw, *se;
    unsigned int x, y, hy, hx, i;
    unsigned int nw1x, nw1y, se1x, se1y;
    unsigned int nw2x, nw2y, se2x, se2y;
    unsigned int nw3x, nw3y, se3x, se3y;
    unsigned int nw4x, nw4y, se4x, se4y;

    QUADTREE_POINT points[4];

    // Generate the new boundaries
    x = n->region.nw.x;
    y = n->region.nw.y;
    hx = n->region.se.x + 1;
    hx -= x;
    hy = n->region.se.y + 1;
    hy -= y;
    hx /= 2; hy /= 2;

    // Create the new regions
    nw1x = x;
    nw1y = y;
    se1x = x + hx - 1;
    se1y = y + hy - 1;  // Validated

    nw2x = x + hx;
    nw2y = y;
    se2x = x + hx + hx - 1;
    se2y = y + hy - 1;

    nw3x = x;
    nw3y = y + hy;
    se3x = x + hx - 1;
    se3y = y + hy + hy - 1;

    nw4x = x + hx;
    nw4y = y + hy;
    se4x = x + hx + hx - 1;
    se4y = y + hy + hy - 1;
/*
    fprintf(stderr, "Subdividing... %p %u %u %u %u\n", n,
        n->region.nw.x, n->region.nw.y,
        n->region.se.x, n->region.se.y
        );
    fprintf(stderr, "%u %u %u %u\n", nw1x, nw1y, se1x, se1y);
    fprintf(stderr, "%u %u %u %u\n", nw2x, nw2y, se2x, se2y);
    fprintf(stderr, "%u %u %u %u\n", nw3x, nw3y, se3x, se3y);
    fprintf(stderr, "%u %u %u %u\n", nw4x, nw4y, se4x, se4y);
*/
    if (_quadtree_node_init(&nw, nw1x, nw1y, se1x, se1y)) return 1;
    if (_quadtree_node_init(&ne, nw2x, nw2y, se2x, se2y)) return 1;
    if (_quadtree_node_init(&sw, nw3x, nw3y, se3x, se3y)) return 1;
    if (_quadtree_node_init(&se, nw4x, nw4y, se4x, se4y)) return 1;

    // Assign the new nodes
    n->nw = nw;
    n->ne = ne;
    n->sw = sw;
    n->se = se;

    for (i = 0; i < 4; i++) {
        points[i] = n->points[i];
    }

    memset(n->points, 0xFFFFFFFF, sizeof(QUADTREE_POINT) * 4);

    for (i = 0; i < 4; i++) {
        if(!_quadtree_insert(n, points[i].x, points[i].y, 0)) {
            return 1;
        }
    }

    return 0;
}

int _quadtree_insert_into_internal(QUADTREE_NODE *n, unsigned int x, unsigned int y) {
    char node_test[sizeof(QUADTREE_POINT)];
    int i;
    // If there is space in this node, add the object here
    memset(node_test, 0xFFFFFFFF, sizeof(QUADTREE_POINT));
    for (i = 0; i < 4; i++) {
        if (memcmp(node_test, n->points + i, sizeof(QUADTREE_POINT))) {
            continue;
        }
        n->points[i] = (QUADTREE_POINT) {x, y};
        return 1;
    }
    return 0;
}

int _quadtree_node_isleaf(QUADTREE_NODE *n) {
    int ret = 1;
    ret &= (n->nw == NULL);
    ret &= (n->ne == NULL);
    ret &= (n->sw == NULL);
    ret &= (n->se == NULL);
    /*fprintf(stderr, "isleaf: %p %d\n", n, ret);*/
    return ret;
}

int _quadtree_insert(QUADTREE_NODE *n, unsigned int x, unsigned int y, int subdivide) {
    // Unit test: test_quadtree_insert_simple.c, test_quadtree_subdivide_1.c
    char node_test[sizeof(QUADTREE_POINT)];

    // If this point is not within the boundary of this node, it can't be added
    if (!_quadtree_node_contains(n, x, y)) return 0;

    if (_quadtree_node_isleaf(n)) {
        if(_quadtree_insert_into_internal(n, x, y)) return 1;
        // Otherwise, subdivide the tree if permitted...
        if (subdivide) {
            if (_quadtree_node_subdivide(n)) {
                fprintf(stderr, "Subdivision error!\n");
                return 0;
            }
        }
    }

    // And add it to the right node
    if (_quadtree_insert(n->nw, x, y, subdivide)) return 1;
    if (_quadtree_insert(n->ne, x, y, subdivide)) return 1;
    if (_quadtree_insert(n->sw, x, y, subdivide)) return 1;
    if (_quadtree_insert(n->se, x, y, subdivide)) return 1;


    fprintf(stderr, "insertion failed %u %u \n", x, y);
    return 0;
}

int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y) {
    return _quadtree_insert(tree->root, x, y, 1);
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
    return _quadtree_query(tree->root, x, y);
}

int _quadtree_scan_x(QUADTREE_NODE *n, unsigned int x, unsigned int *out, unsigned int *p, size_t arr_size) {

    QUADTREE_POINT *point;
    int i;

    // Check the boundaries
    if (x < n->region.nw.x) return 0;
    if (x > n->region.se.x) return 0;

    if (!_quadtree_node_isleaf(n)) {
        // Recurse to children nodes
        if(_quadtree_scan_x(n->nw, x, out, p, arr_size)) return 1;
        if(_quadtree_scan_x(n->ne, x, out, p, arr_size)) return 1;
        if(_quadtree_scan_x(n->sw, x, out, p, arr_size)) return 1;
        if(_quadtree_scan_x(n->se, x, out, p, arr_size)) return 1;
        return 0;
    }

    for (i = 0; i < 4; i++) {
        // Check each point in this leaf
        point = n->points + i;
        if (point->x != x) continue;
        if (*p < arr_size) {
            *(out + *p) = point->y;
            *p = *p + 1;
        }
        else {
            *p = *p + 1;
            return 1; // Need a bigger array to complete the scan
        }
    }

    return 0;

}

int _quadtree_scan_y(QUADTREE_NODE *n, unsigned int y, unsigned int *out, unsigned int *p, size_t arr_size) {

    QUADTREE_POINT *point;
    int i;

    // Check the boundaries
    if (y < n->region.nw.y) return 0;
    if (y > n->region.se.y) return 0;

    if (!_quadtree_node_isleaf(n)) {
        // Recurse to children nodes
        if(_quadtree_scan_y(n->nw, y, out, p, arr_size)) return 1;
        if(_quadtree_scan_y(n->ne, y, out, p, arr_size)) return 1;
        if(_quadtree_scan_y(n->sw, y, out, p, arr_size)) return 1;
        if(_quadtree_scan_y(n->se, y, out, p, arr_size)) return 1;
        return 0;
    }

    for (i = 0; i < 4; i++) {
        // Check each point in this leaf
        point = n->points + i;
        if (point->y != y) continue;
        if (*p < arr_size) {
            *(out + *p) = point->x;
            *p = *p + 1;
        }
        else {
            *p = *p + 1;
            return 1; // Need a bigger array to complete the scan
        }
    }

    return 0;

}

int quadtree_scan_x(QUADTREE *tree, unsigned int x, unsigned int *out, unsigned int *p, size_t arr_size) {
    return _quadtree_scan_x(tree->root, x, out, p, arr_size);
}

int quadtree_scan_y(QUADTREE *tree, unsigned int y, unsigned int *out, unsigned int *p, size_t arr_size) {
    return _quadtree_scan_y(tree->root, y, out, p, arr_size);
}