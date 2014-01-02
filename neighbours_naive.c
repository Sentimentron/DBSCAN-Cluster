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

int compute_ui_bitvec (
    unsigned int *arr,              // Arrays to compare
    unsigned int  len,              // Largest element in each array
    char *buf, 
    unsigned int bufsize        // Working buffers, and their size in bits
    ) {

    // Translate buf1 and buf2 into bitvectors
    unsigned int i;

    // Clear buffer
    memset(buf, 0, (bufsize+7)/8);

    // Convert array to bit offset 
    for (i = 0; i < len; i++) {
        unsigned int point = *(arr + i);
        BITVEC_INS(point, buf);
    }

    return 0;
}

int compute_intersection (
    char *buf1, char *buf2, 
    unsigned int buf1size,
    unsigned int buf2size
    )
{
    int ret, min, i;
    char tmp;

    min = buf2size;
    if (buf1size < buf2size) min = buf1size;

    // Compute the union of buf1 and buf2 
    for (i = 0, ret = 0; i < (min+7)/8; i++) {
        tmp = *(buf1 + i) & *(buf2 + i);
        ret += popcount(tmp);
    }

    return ret; 
}

int compute_union(
    char *buf1, char *buf2, 
    unsigned int buf1size,
    unsigned int buf2size
    )
{
    int ret, min, max, i;
    char tmp, *maxbuf;

    max = buf2size;
    if (buf1size > buf2size) max = buf1size;
    min = buf2size;
    
    maxbuf = buf2;
    if (buf2size > buf1size) {
        min = buf1size;
        maxbuf = buf1;
    }

    // Compute the union of buf1 and buf2 
    for (i = 0, ret = 0; i < (min+7)/8; i++) {
        tmp = *(buf1 + i) | *(buf2 + i);
        ret += popcount(tmp);
    }

    // Correct the union by recording the popcount of the longest
    for (i = (min/8) + 1; i < (max+7)/8; i++) {
        ret += popcount(*(maxbuf + i));
    }

    return ret; 
}

int neighbours_determine_initial_set(char *out, void *dptr, 
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
            if(!(BITVEC_SET(point, out))) ret_size++;
            BITVEC_INS(point, out);
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
    char *out, void *dptr, 
    unsigned int current_point,
    float eps, unsigned int *count
    ) {

    QUADTREE *data = (QUADTREE *)dptr;
    unsigned int labels[5], size; 
    char buf1, buf2;

    // Get the initial set 
    if (neighbours_determine_initial_set(out, dptr, current_point, count)) return 1;

    // Read source labels
    size = 0;
    assert(!quadtree_scan_x(data, current_point, labels, &size, 5));

    // Convert source labels
    assert(!compute_ui_bitvec(labels, size, &buf1, 8));

    // For each point set in that set
    for (int i = 0; i < 8; i++) {
        float inter, uni;
        float distance; 
        if (!(BITVEC_SET(i, out))) continue;
        // Read labels 
        unsigned int labels2[5], labels_size2 = 0;
        assert(!quadtree_scan_x(data, i, labels2, &labels_size2, 5));

        // Compute bitvectors
        assert(!compute_ui_bitvec(labels2, labels_size2, &buf2, 8));

        // Compute intersection
        inter = compute_intersection(&buf1, &buf2, 8, 8);

        // Compute union
        uni = compute_union(&buf1, &buf2, 8, 8);
        
        // Compute distance measure 
        distance = 1.0f - (float)inter / uni;

        // If greater than epsilon
        if (distance > eps) {
            // Clear output bit
            BITVEC_CLEAR(i, out);
            *count = *count - 1;
        }
    }

    return 0;
}