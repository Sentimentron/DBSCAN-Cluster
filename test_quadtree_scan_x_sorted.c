#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "quadtree.h"

int quadtree_scan_x(QUADTREE *tree, unsigned int x, unsigned int *out, unsigned int *p, size_t arr_size);

unsigned int nextpow2(int of) {
    unsigned int ret;
    for (ret = 1; ret < of; ret *= 2);
    return ret;
}

int max(int *arr, int len) {
    int i, max;
    for (i = 0, max = 0; i < len; i++) {
        if (*(arr + i) > max) {
            max = *(arr + i);
        }
    }
    return max;
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int comp(int *arg1, unsigned int *arg2, int length1, int length2) {
    // Compare the original random array with the one scanned
    // Return 1 if they contain the same elements, 0 otherwise
    int i, j;
    for (i = 0; i < length1; i++) {
        int icur = arg1[i];
        for (j = 0; j < length2; j++) {
            int jcur = arg2[j];
            if (jcur == icur) {
                arg2[j] = 0;
            }
        }
    }

    for (i = 0; i < length2; i++) {
        if (arg2[i]) {
            return 0;
        }
    }

    return 1;
}

int sort_issorted(unsigned int *arr, unsigned int items) {
    unsigned cur = 0;
    for (unsigned int i = 0; i < items - (items % 4); i++) {
        if (arr[i] < cur) {
            return 0;
        }
        cur = arr[i];
    }
    return 1;
}

int main(int argc, char **argv) {

    unsigned int random0[64], random1[63], random2[62], random3[61], i, j;
    unsigned int s, p, scan0[64], scan1[64],scan2[64],scan3[64];
    QUADTREE *ref = NULL;

    // Configure random numbers
    srand(time(NULL));

    // Generate 64 of your finest random0 numbers!
    for (i = 0; i < 64; i++) {
        int dup = 1;
        do {
            random0[i] = rand();
            // Check this one isn't a duplicate
            for (j = 0, dup = 0; j < i; j++) {
                if (random0[j] == random0[i]) {
                    dup = 1; continue;
                }
            }
        }
        while(dup);
    }

    // Copy the random numbers
    memcpy(random1, random0, 63 * sizeof(int));
    memcpy(random2, random0, 62 * sizeof(int));
    memcpy(random3, random0, 61 * sizeof(int));

    // Get the maximum random number generated
    s = max(random0, 64);

    // Get the nearest 2 power
    s = nextpow2(s);

    // Generate the tree
    assert(!quadtree_init(&ref, s, s));

    for (i = 0; i < 64; i++) {
        // Insert the random numbers at a constant y value
        assert(quadtree_insert(ref, 13, random0[i]));
    }

    qsort(random0, 64, sizeof(int), cmpfunc);
    assert(sort_issorted(random0, 64));

    p = 0;
    assert(!quadtree_scan_x(ref, 13, scan0, &p, 64));
    assert(sort_issorted(scan0, 64));

    p = 0;
    quadtree_scan_x(ref, 13, scan1, &p, 63);
    assert(sort_issorted(scan1, 63));

    p = 0;
    quadtree_scan_x(ref, 13, scan2, &p, 62);
    assert(sort_issorted(scan2, 62));

    p = 0;
    quadtree_scan_x(ref, 13, scan3, &p, 61);
    assert(sort_issorted(scan3, 61));

    return 0;
}