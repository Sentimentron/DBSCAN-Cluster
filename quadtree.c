#include <stdio.h>
#include <stdlib.h>

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

int _qp_alloc(QUADTREE_POINT **ref) {
    // Allocate a QUADTREE_POINT and return i
    NULLCHECK(ref);
    SAFE_MALLOC(sizeof(QUADTREE_POINT), 1, ref);
}