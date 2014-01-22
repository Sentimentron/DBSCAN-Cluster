#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "quadtree.h"
#include "bitvec.h"

static bitvec_t *_NN_GLOB_BUF1 = NULL;
static bitvec_t *_NN_GLOB_BUF2 = NULL;

static unsigned int *_NN_GLOB_LABEL_BUF = NULL;
static unsigned int _NN_GLOB_LABEL_BUF_SIZE = 0;

static unsigned int *_NN_GLOB_ID_BUF = NULL;
static unsigned int _NN_GLOB_ID_BUF_SIZE = 0;

static 

void _lab_realloc(unsigned int size) {
    if (size < _NN_GLOB_LABEL_BUF_SIZE) return;
    _NN_GLOB_LABEL_BUF = realloc(_NN_GLOB_LABEL_BUF, size);
    if (_NN_GLOB_LABEL_BUF == NULL) {
        fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
        exit(2);
    }
    _NN_GLOB_LABEL_BUF_SIZE = size;
}

void _id_realloc(unsigned int size) {
    if (size < _NN_GLOB_ID_BUF_SIZE) return;
    _NN_GLOB_ID_BUF = realloc(_NN_GLOB_ID_BUF, size);
    if (_NN_GLOB_ID_BUF == NULL) {
        fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
        exit(2);
    }
    _NN_GLOB_ID_BUF_SIZE = size;
}

unsigned int _get_maximum_element (unsigned int *arr, unsigned int len) {
    unsigned int i, ret;
    for (i = 0, ret = 0; i < len; i++) {
        if (*(arr + i) > ret) ret = *(arr + i);
    }
    return ret;
}

int neighbours_determine_initial_set(
    bitvec_t *out, void *dptr,
    unsigned int current_point,
    unsigned int *count
    ) {

    QUADTREE *data = (QUADTREE *)dptr;
    unsigned int status, size, ret_size, numpoints, cur, i, j;

    // Read the labels of this x-coordinate into the label buffer
    for(status = 0; ;) {
        size = 0;
        status = quadtree_scan_x (
            data, current_point,
            _NN_GLOB_LABEL_BUF, &size,
            _NN_GLOB_LABEL_BUF_SIZE
        );
        if (status) {
            // Indicates we're out of memory and need to resize
            _lab_realloc(size);
            continue;
        }
        break;
    }

    // Find other points with those labels
    ret_size = 0;
    for (i = 0; i < size; i++) {
        // Read the points from the quadtree
        for (status = 0, cur = _NN_GLOB_LABEL_BUF[i]; ;) {
            numpoints = 0;
            status = quadtree_scan_y (
                data, cur,
                _NN_GLOB_ID_BUF, &numpoints,
                _NN_GLOB_ID_BUF_SIZE
            );
            if (status) {
                _id_realloc(numpoints);
                continue;
            }
            break;
        }
        for (j = 0; j < numpoints; j++) {
            unsigned int point = _NN_GLOB_ID_BUF[j];
            if (!(bitvec_check(out, point))) ret_size++;
            bitvec_set(out, point);
        }
    }

    *count = ret_size;

    return 0;
}


unsigned int neighbours_search (
    bitvec_t *out, void *dptr,
    unsigned int current_point,
    float eps, unsigned int *count
    ) {

    unsigned int status, size;
    QUADTREE *data = (QUADTREE *)dptr;

    if (_NN_GLOB_BUF1 == NULL) {
        bitvec_alloc(&_NN_GLOB_BUF1, 8);
    }
    if (_NN_GLOB_BUF2 == NULL) {
        bitvec_alloc(&_NN_GLOB_BUF2, 8);
    }

    bitvec_clear_all(_NN_GLOB_BUF1);
    bitvec_clear_all(_NN_GLOB_BUF2);

    // Mark the candidate set in the out vector
    if (neighbours_determine_initial_set(out, dptr, current_point, count)) return 1;

    // Labels for this point are now in _NN_GLOB_LABEL_BUF, so we don't need
    // to worry about reading them again as in neighbours_naive.c
    // Every point which has one of those labels is now marked in out

    for (status = 0; ;) {
        size = 0;
        status = quadtree_scan_x (
            data, current_point,
            _NN_GLOB_LABEL_BUF, &size,
            _NN_GLOB_LABEL_BUF_SIZE
        );
        if (status) {
            _lab_realloc(size);
        }
        break;
    }

    // Convert the x-coordinate labels into a bit-vector
    bitvec_batch_set_u32(_NN_GLOB_BUF1, _NN_GLOB_LABEL_BUF, size);

    for (unsigned int i = 0; i < out->max_offset; i++) {
        float distance;
        if (!bitvec_check(out, i)) continue;
        if (i) bitvec_clear_all(_NN_GLOB_BUF2);

        // Read labels
        for(status = 0; ;) {
            size = 0;
            status = quadtree_scan_x (
                data, i,
                _NN_GLOB_LABEL_BUF, &size,
                _NN_GLOB_LABEL_BUF_SIZE
            );
            if (status) {
                // Indicates we're out of memory and need to resize
                _lab_realloc(size);
                continue;
            }
            break;
        }

        // Convert labels
        bitvec_batch_set_u32(_NN_GLOB_BUF2, _NN_GLOB_LABEL_BUF, size);

        // Compute distance measure
        distance = bitvec_distance(_NN_GLOB_BUF1, _NN_GLOB_BUF2);

        // If greater than epsilon
        if (distance > eps) {
            // Clear output bit
            bitvec_clear(out, i);
            *count = *count - 1;
        }
    }

    return 0;
}