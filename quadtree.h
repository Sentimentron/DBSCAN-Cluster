#ifndef __QUADTREE_H__
#define __QUADTREE_H__

typedef struct {
    unsigned int x, y;
} QUADTREE_POINT;

typedef struct {
    QUADTREE_POINT nw, se; 
    unsigned int width, height;
} QUADTREE_REGION;

typedef struct _QUADTREE_NODE {
    struct _QUADTREE_NODE *nw, *ne, *sw, *se;
    QUADTREE_REGION region;
    QUADTREE_POINT points[4];
} QUADTREE_NODE;

typedef struct {
    QUADTREE_NODE *root;
} QUADTREE;

int quadtree_query (QUADTREE *tree, unsigned int x, unsigned int y);
int quadtree_insert(QUADTREE *tree, unsigned int x, unsigned int y);
int quadtree_init(QUADTREE **ref, unsigned int xmax, unsigned int ymax);

int quadtree_scan_x(QUADTREE *tree, unsigned int x, unsigned int *out, unsigned int *p, size_t arr_size);
int quadtree_scan_y(QUADTREE *tree, unsigned int x, unsigned int *out, unsigned int *p, size_t arr_size);

#endif