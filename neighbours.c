#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#define BITVEC_INS(off, bitvec) bitvec[off / 8] |= (1 << (off % 8))
#define BITVEC_SET(off, bitvec) bitvec[off / 8] & (1 << (off % 8))
#define BITVEC_CLEAR(off, bitvec) bitvec[off / 8] &= ~(1 << (off % 8))

static char *_NN_GLOB_BUF1 = NULL;
static char *_NN_GLOB_BUF2 = NULL;
static unsigned int _NN_GLOB_BUF1_SIZE = 0;
static unsigned int _NN_GLOB_BUF2_SIZE = 0;

static unsigned int *_NN_GLOB_LABEL_BUF = NULL;
static unsigned int _NN_GLOB_LABEL_BUF_SIZE = 0;

static unsigned int *_NN_GLOB_ID_BUF = NULL;
static unsigned int _NN_GLOB_ID_BUF_SIZE = 0;

void _bv_realloc(unsigned int s1, unsigned int s2) {

    s1 = (s1 + 7) / 8;
    s2 = (s2 + 7) / 8;

    if (_NN_GLOB_BUF1_SIZE < s1) {
        _NN_GLOB_BUF1 = realloc(_NN_GLOB_BUF1, s1);
        _NN_GLOB_BUF1_SIZE = s1;
        if (_NN_GLOB_BUF1 == NULL) {
            fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
            exit(2);
        }
    }
    if (_NN_GLOB_BUF2_SIZE < s2) {
        _NN_GLOB_BUF2 = realloc(_NN_GLOB_BUF2, s2);
        _NN_GLOB_BUF2_SIZE = s2;
        if (_NN_GLOB_BUF2 == NULL) {
            fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
            exit(2);
        }
    }

}

void _bv_clear() {
    memset(_NN_GLOB_BUF1, 0, _NN_GLOB_BUF1_SIZE);
    memset(_NN_GLOB_BUF1, 0, _NN_GLOB_BUF1_SIZE);
}

void _lab_realloc(unsigned int size) {
    _NN_GLOB_LABEL_BUF = realloc(_NN_GLOB_LABEL_BUF, size);
    if (_NN_GLOB_LABEL_BUF == NULL) {
        fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
        exit(2);
    }
    _NN_GLOB_LABEL_BUF_SIZE = size;
}

void _id_realloc(unsigned int size) {
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
    char *out, void *dptr, 
    unsigned int current_point,
    unsigned int *count 
    ) {

    QUADTREE *data = (QUADTREE *)dptr;
    unsigned int status, size, ret_size, max, numpoints, cur, i, j;

    // Read the labels of this x-coordinate into the label buffer
    for(status = 0, size = 0; ;) {
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
        for (status = 0, numpoints = 0, cur = _NN_GLOB_LABEL_BUF[i]; ;) {
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
        // Mark these points in the out vector
        for (j = 0; j < numpoints; j++) {
            cur = _NN_GLOB_ID_BUF[i];
            if(!(BITVEC_SET(point, out))) ret_size++;
            BITVEC_INS(point, out);
        }
    }

    *count = ret_size;
    return 0;
}

unsigned int neighbours_search (
    char *out, void *dptr, 
    unsigned int current_point,
    float eps, unsigned int len
    unsigned int *count
    ) {

    QUADTREE *data = (QUADTREE *)dptr;

    // Mark the candidate set in the out vector
    if (neighbours_determine_initial_set(out, dptr, current_point, count)) return 1;

    // Labels for this point are now in _NN_GLOB_LABEL_BUF, so we don't need
    // to worry about reading them again as in neighbours_naive.c

    // Find the maximum label pulled out
    max = _get_maximum_element(_NN_GLOB_LABEL_BUF, _NN_GLOB_LABEL_BUF_SIZE);

    // Resize the label bit-vectors to at least that size
    _bv_realloc(max, max);

    // Convert the x-coordinate labels into a bit-vector
    if (compute_ui_bitvec(_NN_GLOB_LABEL_BUF, size, _NN_GLOB_BUF1, _NN_GLOB_BUF1_SIZE)) {
        return 1;
    }

    for (int i = 0; i < *count; i++) {
        float inter, uni, distance;
        if (!(BITVEC_SET(i, out))) continue; 
        // Read labels 
        for(status = 0, size = 0; ;) {
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
        max = _get_maximum_element(_NN_GLOB_LABEL_BUF, _NN_GLOB_LABEL_BUF_SIZE);
        // Convert the x-coordinate labels into a bit-vector
        if (compute_ui_bitvec(_NN_GLOB_LABEL_BUF, size, _NN_GLOB_BUF2, _NN_GLOB_BUF2_SIZE)) {
            return 1;
        }

        inter = compute_intersection(_NN_GLOB_BUF1, _NN_GLOB_BUF2, _NN_GLOB_BUF1_SIZE, _NN_GLOB_BUF2_SIZE);
        uni = compute_union(_NN_GLOB_BUF1, _NN_GLOB_BUF2, _NN_GLOB_BUF1_SIZE, _NN_GLOB_BUF2_SIZE);

        distance = 1.0f - inter / uni; 

        // If greater than epsilon
        if (distance > eps) {
            // Clear output bit
            BITVEC_CLEAR(i, out);
            *count = *count - 1;
        }
    }
}