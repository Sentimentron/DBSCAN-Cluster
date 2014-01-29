#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "quadtree.h"
#include "bitvec.h"

static bitvec_t *_NN_GLOB_BUF1 = NULL;
static bitvec_t *_NN_GLOB_BUF2 = NULL;

static unsigned int *_NN_GLOB_LABEL_BUF1 = NULL;
static unsigned int _NN_GLOB_LABEL_BUF1_SIZE = 0;

static unsigned int *_NN_GLOB_LABEL_BUF2 = NULL;
static unsigned int _NN_GLOB_LABEL_BUF2_SIZE = 0;

static unsigned int *_NN_GLOB_ID_BUF = NULL;
static unsigned int _NN_GLOB_ID_BUF_SIZE = 0;

static QUADTREE *_NN_GLOB_CACHED_QUADTREE = NULL;
static unsigned int *_NN_GLOB_QUADTREE_CACHE = NULL;
static unsigned int _NN_GLOB_QUADTREE_CACHE_MAX_LABEL_COUNT = 0;

void _lab1_realloc(unsigned int size) {
    if (size < _NN_GLOB_LABEL_BUF1_SIZE) return;
    _NN_GLOB_LABEL_BUF1 = realloc(_NN_GLOB_LABEL_BUF1, size * sizeof(unsigned int));
    if (_NN_GLOB_LABEL_BUF1 == NULL) {
        fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
        exit(2);
    }
    _NN_GLOB_LABEL_BUF1_SIZE = size;
}

void _lab2_realloc(unsigned int size) {
    if (size < _NN_GLOB_LABEL_BUF2_SIZE) return;
    _NN_GLOB_LABEL_BUF2 = realloc(_NN_GLOB_LABEL_BUF2, size * sizeof(unsigned int));
    if (_NN_GLOB_LABEL_BUF2 == NULL) {
        fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
        exit(2);
    }
    _NN_GLOB_LABEL_BUF2_SIZE = size;
}

void _id_realloc(unsigned int size) {
    if (size < _NN_GLOB_ID_BUF_SIZE) return;
    _NN_GLOB_ID_BUF = realloc(_NN_GLOB_ID_BUF, size *  sizeof(unsigned int));
    if (_NN_GLOB_ID_BUF == NULL) {
        fprintf(stderr, "%s:%d: Allocation error", __FILE__, __LINE__);
        exit(2);
    }
    _NN_GLOB_ID_BUF_SIZE = size;
}

int neighbours_determine_initial_set(
    bitvec_t *out, void *dptr,
    unsigned int current_point,
    unsigned int *count
    ) {

    QUADTREE *data = (QUADTREE *)dptr;
    unsigned int status, size, ret_size, numpoints, cur, i, j;
    unsigned int *labels;

    // Read the labels of this x-coordinate into the label buffer
    labels = (_NN_GLOB_QUADTREE_CACHE + current_point * (_NN_GLOB_QUADTREE_CACHE_MAX_LABEL_COUNT + 1));
    size = *labels;
    labels++;

    // Find other points with those labels
    ret_size = 0;
    for (i = 0; i < size; i++) {
        // Read the points from the quadtree
        for (status = 0, cur = labels[i]; ;) {
            numpoints = 0;
            status = quadtree_scan_y (
                data, cur,
                _NN_GLOB_ID_BUF, &numpoints,
                _NN_GLOB_ID_BUF_SIZE
            );
            if (status) {
                _id_realloc(numpoints * 2);
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

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)b - *(int*)a );
}

float neighbours_distance (
    unsigned int *a,
    unsigned int sa,
    unsigned int *b,
    unsigned int sb
    )
{
    unsigned int i = 0, j = 0, in = 0, u;

    if ((sa == sb) && sa == 0) {
        return 1.0;
    }

    u = sa + sb; // Initial estimate
    while (i < sa && j < sb) {
        if (a[i] == b[j]) {
            in++;
            u--;
            i++;
            j++;
        }
        else if (a[i] < b[j]) {
            j++;
        }
        else {
            i++;
        }
    }

    //fprintf(stderr, "%f\n", 1.0 - (float)in/u);
    return 1.0 - (float)in/u;
}

unsigned int _build_quadtree_x_map(QUADTREE *data, unsigned int len) {
    unsigned int i, status, size, max_size, *off;
    // No need to recompute!
    if (data == _NN_GLOB_CACHED_QUADTREE) return 0;

    fprintf(stderr, "Building quadtree cache... (finding maximum label count)\n");
    for (i = 0, max_size = 0; i < len; i++) {
        for (status = 0; ;) {
            size = 0;
            status = quadtree_scan_x (
                data, i,
                _NN_GLOB_LABEL_BUF1, &size,
                _NN_GLOB_LABEL_BUF1_SIZE
            );
            if (status) {
                _lab1_realloc(size);
                continue;
            }
            if (size > max_size) max_size = size;
            break;
        }
    }

    fprintf(stderr, "Building quadtree cache... (allocating...)\n");
    _NN_GLOB_QUADTREE_CACHE = malloc(sizeof(unsigned int) * (max_size + 1) * len);
    if (_NN_GLOB_QUADTREE_CACHE == NULL) {
        fprintf(stderr, "Allocation failure!\n");
        exit(2);
    }

    fprintf(stderr, "Building quadtree cache... (caching...)\n");
    for (i = 0; i < len; i++) {
        off = _NN_GLOB_QUADTREE_CACHE + (i * (max_size + 1));
        *off = 0;
        status = quadtree_scan_x (data, i, off+1, off, max_size);
        assert(!status);
    }

    _NN_GLOB_QUADTREE_CACHE_MAX_LABEL_COUNT = max_size;
    _NN_GLOB_CACHED_QUADTREE = data;
    fprintf(stderr, "Done!\n");
    return 0;
}

unsigned int neighbours_search (
    bitvec_t *out, void *dptr,
    unsigned int current_point,
    float eps, unsigned int *count
    ) {

    unsigned int status, size1, size2, *labels1, *labels2;
    QUADTREE *data = (QUADTREE *)dptr;
    _lab1_realloc(2048);
    _lab2_realloc(2048);
    _id_realloc(2048);


    _build_quadtree_x_map(data, out->max_offset);

    // Mark the candidate set in the out vector
    if (neighbours_determine_initial_set(out, dptr, current_point, count)) return 1;

    // Labels for this point are now in _NN_GLOB_LABEL_BUF1, so we don't need
    // to worry about reading them again as in neighbours_naive.c
    // Every point which has one of those labels is now marked in out

    // Fetch the labels for this point
    labels1 = _NN_GLOB_QUADTREE_CACHE + (current_point * (_NN_GLOB_QUADTREE_CACHE_MAX_LABEL_COUNT + 1));
    size1 = *labels1;
    labels1++;

    for (unsigned int i = 0; i < out->max_offset; i = bitvec_get_next_offset(out, i)) {
        float distance;
        if (!bitvec_check(out, i)) continue;
        //fprintf(stderr, "Neighbours: %.2f\n", 100.0 * i / out->max_offset);

        labels2 = _NN_GLOB_QUADTREE_CACHE + (i * (_NN_GLOB_QUADTREE_CACHE_MAX_LABEL_COUNT + 1));
        size2 = *labels2;
        labels2++;

        // Compute distance measure
        distance = neighbours_distance (
            labels1, size1,
            labels2, size2
        );

        // If greater than epsilon
        if (distance > eps) {
            // Clear output bit
            bitvec_clear(out, i);
            *count = *count - 1;
        }
        i++;
    }

    return 0;
}
