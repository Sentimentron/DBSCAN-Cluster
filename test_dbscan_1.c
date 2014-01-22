#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "quadtree.h"
#include "bitvec.h"
#include "dbscan.h"

int main(int argc, char **argv) {

    bitvec_t *neighbours;
    unsigned int c;
    QUADTREE *ref = NULL; 

    bitvec_alloc(&neighbours, 8);

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
    assert(!neighbours_determine_initial_set(neighbours, ref, 0, &c));
    assert(c == 3);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_determine_initial_set(neighbours, ref, 1, &c));
    assert(c == 3);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_determine_initial_set(neighbours, ref, 2, &c));
    assert(c == 3);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_determine_initial_set(neighbours, ref, 3, &c));
    assert(c == 1);
    assert(bitvec_check(neighbours, 3));

    // Test the neighbour generation algorithm
    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 0, 0.005f, &c));
    assert(c == 2);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 1, 0.005f, &c));
    assert(c == 2);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 2, 0.005f, &c));
    assert(c == 1);
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 3, 0.005f, &c));
    assert(c == 1);
    assert(bitvec_check(neighbours, 3));


    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 0, 0.5f, &c));
    assert(c == 3);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 1, 0.5f, &c));
    assert(c == 3);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 2, 0.5f, &c));
    assert(c == 3);
    assert(bitvec_check(neighbours, 0));
    assert(bitvec_check(neighbours, 1));
    assert(bitvec_check(neighbours, 2));

    c = 0; bitvec_clear_all(neighbours);
    assert(!neighbours_search(neighbours, ref, 3, 0.5f, &c));
    assert(c == 1);
    assert(bitvec_check(neighbours, 3));

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