#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "quadtree.h"

#define BITVEC_INS(off, bitvec) bitvec[off / 8] |= (1 << (off % 8))
#define BITVEC_SET(off, bitvec) bitvec[off / 8] & (1 << (off % 8))
#define BITVEC_CLEAR(off, bitvec) bitvec[off / 8] &= ~(1 << (off % 8))

#define popcount(of)  __builtin_popcount(of)


int DBSCAN(void *data, unsigned int *d, unsigned int dlen,
            float eps, unsigned int minpoints,
            unsigned int (*neighbours_search)(char *out, 
            void *, unsigned int, float, unsigned int *)
    );

unsigned int neighbours_search (
    char *out, void *dptr, 
    unsigned int current_point,
    float eps, unsigned int *count
    );

int main(int argc, char **argv) {

    char neighbours;
    unsigned int c;
    QUADTREE *ref = NULL; 

    unsigned int clusters[4];
    unsigned int expected_clusters1[] = {1, 1, 0, 0};
    unsigned int expected_clusters2[] = {1, 1, 1, 0};
    unsigned int expected_clusters3[] = {0, 0, 0, 0};
    unsigned int expected_clusters4[] = {1, 1, 1, 0};

    assert(!quadtree_init(&ref, 7, 7));
    // x is the document, y is the label
    assert(quadtree_insert(ref, 0, 4));
    assert(quadtree_insert(ref, 1, 4));
    assert(quadtree_insert(ref, 2, 3));
    assert(quadtree_insert(ref, 2, 4));
    assert(quadtree_insert(ref, 3, 1));

    // Test the initial set algorithm
    assert(!neighbours_determine_initial_set(&neighbours, ref, 0, &c));
    assert(c == 3);
    assert(neighbours == 0x7);

    c = 0; neighbours = 0;
    assert(!neighbours_determine_initial_set(&neighbours, ref, 1, &c));
    assert(c == 3);
    assert(neighbours == 0x7);

    c = 0; neighbours = 0;
    assert(!neighbours_determine_initial_set(&neighbours, ref, 2, &c));
    assert(c == 3);
    assert(neighbours == 0x7);

    c = 0; neighbours = 0;
    assert(!neighbours_determine_initial_set(&neighbours, ref, 3, &c));
    assert(c == 1);
    assert(neighbours == 0x8);

    // Test the neighbour generation algorithm
    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 0, 0.005f, &c));
    assert(c == 2);
    assert(neighbours == 0x3);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 1, 0.005f, &c));
    assert(c == 2);
    assert(neighbours == 0x3);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 2, 0.005f, &c));
    assert(c == 1);
    assert(neighbours == 0x4);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 3, 0.005f, &c));
    assert(c == 1);
    assert(neighbours == 0x8);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 0, 0.5f, &c));
    assert(c == 3);
    assert(neighbours == 0x7);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 1, 0.5f, &c));
    assert(c == 3);
    assert(neighbours == 0x7);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 2, 0.5f, &c));
    assert(c == 3);
    assert(neighbours == 0x7);

    c = 0; neighbours = 0;
    assert(!neighbours_search(&neighbours, ref, 3, 0.5f, &c));
    assert(c == 1);
    assert(neighbours == 0x8);

    for (int i = 0; i < 4; i++) clusters[i] = i;
    assert(!DBSCAN(ref, clusters, 4, 0.005f, 2, &neighbours_search));
    assert(!memcmp(clusters, expected_clusters1, sizeof(unsigned int) * 4));

    for (int i = 0; i < 4; i++) clusters[i] = i;
    assert(!DBSCAN(ref, clusters, 4, 0.5, 2, &neighbours_search));
    assert(!memcmp(clusters, expected_clusters2, sizeof(unsigned int) * 4));

    for (int i = 0; i < 4; i++) clusters[i] = i;
    assert(!DBSCAN(ref, clusters, 4, 0.005f, 3, &neighbours_search));
    assert(!memcmp(clusters, expected_clusters3, sizeof(unsigned int) * 4));

    for (int i = 0; i < 4; i++) clusters[i] = i;
    assert(!DBSCAN(ref, clusters, 4, 0.5f, 3, &neighbours_search));
    assert(!memcmp(clusters, expected_clusters4, sizeof(unsigned int) * 4));


    return 0;
}