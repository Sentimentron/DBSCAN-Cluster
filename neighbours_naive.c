#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "bitvec.h"
#include "quadtree.h"

int neighbours_determine_initial_set(bitvec_t *out, void *dptr, 
    unsigned int current_point,
    unsigned int *count
    ) {

    QUADTREE *data = (QUADTREE *)dptr;
    unsigned int lscan[5], pscan[4], ret_size, size; // Only 4 elements to worry about 
    unsigned int i, j;

    // Retrieve the labels that this point has
    size = 0; ret_size = 0;
    assert(!quadtree_scan_x(data, current_point, lscan, &size, 5));
    for (i = 0; i < size; i++) {
        unsigned int numpoints = 0;
        assert(!quadtree_scan_y(data, lscan[i], pscan, &numpoints, 4));
        for (j = 0; j < numpoints; j++) {
            unsigned int point = pscan[j];
            if (!bitvec_check(out, point)) ret_size++;
            bitvec_set(out, point);
        }
    }

    *count = ret_size;

    return 0;
}

unsigned int max (unsigned int *arr, unsigned int len) {
    unsigned int i, ret;
    for (i = 0, ret = 0; i < len; i++) {
        if (*(arr + i) > ret) ret = *(arr + i);
    }
    return ret;
}

unsigned int neighbours_search (
    bitvec_t *out, void *dptr, 
    unsigned int current_point,
    float eps, unsigned int *count
    ) {

    QUADTREE *data = (QUADTREE *)dptr;
    unsigned int labels[5], size; 
    bitvec_t *buf1, *buf2;

    bitvec_alloc(&buf1, 1);
    bitvec_alloc(&buf2, 1);

    // Get the initial set 
    if (neighbours_determine_initial_set(out, dptr, current_point, count)) return 1;

    // Read source labels
    size = 0;
    assert(!quadtree_scan_x(data, current_point, labels, &size, 5));

    // Convert source labels
    bitvec_batch_set_u32(buf1, labels, size);

    // For each point set in that set
    for (int i = 0; i < 8; i++) {
        float inter, uni;
        float distance; 
        if (!(bitvec_check(out, i))) continue;
        if (i) bitvec_clear_all(buf2);

        // Read labels 
        unsigned int labels2[5], labels_size2 = 0;
        assert(!quadtree_scan_x(data, i, labels2, &labels_size2, 5));
        // Convert labels
        bitvec_batch_set_u32(buf2, labels2, labels_size2);
        
        // Compute distance measure 
        distance = bitvec_distance(buf1, buf2);

        // If greater than epsilon
        if (distance > eps) {
            // Clear output bit
            bitvec_clear(out, i);
            *count = *count - 1;
        }
    }

    return 0;
}