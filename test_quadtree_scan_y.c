#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "quadtree.h"

int quadtree_scan_y(QUADTREE *tree, unsigned int x, unsigned int *out, unsigned int *p, size_t arr_size);

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

int main(int argc, char **argv) {

    int random[64], i, j;
    unsigned int s, p, scan[64];
    QUADTREE *ref = NULL; 

    // Configure random numbers 
    srand(time(NULL));

    // Generate 64 of your finest random numbers!
    for (i = 0; i < 64; i++) {
        int dup = 1;
        do {
            random[i] = rand();
            // Check this one isn't a duplicate
            for (j = 0, dup = 0; j < i; j++) {
                if (random[j] == random[i]) {
                    dup = 1; continue;
                }
            }
        }
        while(dup);

    }

    // Get the maximum random number generated
    s = max(random, 64);

    // Get the nearest 2 power 
    s = nextpow2(s);

    // Generate the tree 
    assert(!quadtree_init(&ref, s, s));

    for (i = 0; i < 64; i++) {
        // Insert the random numbers at a constant x value
        assert(quadtree_insert(ref, random[i], 13));
    }

    // Expect a scan along an x-offset with no values
    // will result in a non-incremented pointer
    p = 0;
    assert(!quadtree_scan_y(ref, 15, scan, &p, 64));
    assert(p == 0);

    // Expect a scan along the correct offset without
    // sufficient memory to return error code 1, and
    // a partially-filled search array
    p = 0;
    assert(quadtree_scan_y(ref, 13, scan, &p, 32) == 1);
    assert(comp(random, scan, 64, 32));
    
    // Check for a successful scan
    p = 0;
    assert(!quadtree_scan_y(ref, 13, scan, &p, 64));
    assert(comp(random, scan, 64, 64));

    for (i = 0; i < 64; i++) {
        // Insert the random numbers at a constant x value
        assert(quadtree_insert(ref, random[i], 0));
    }

    p = 0;
    assert(!quadtree_scan_y(ref, 0, scan, &p, 64));
    assert(comp(random, scan, 64, 64));

    for (i = 0; i < 64; i++) {
        // Insert the random numbers at a constant x value
        assert(quadtree_insert(ref, random[i], s - 1));
    }

    p = 0;
    assert(!quadtree_scan_y(ref, s-1, scan, &p, 64));
    assert(comp(random, scan, 64, 64));

    return 0;
}