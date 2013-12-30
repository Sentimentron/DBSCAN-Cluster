#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>


#include "quadtree.h"

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

int main(int argc, char **argv) {

    int random[64], i, j;
    unsigned int s;
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
        // Insert the random number 
        assert(quadtree_insert(ref, random[i], random[i]));
    }

    for (i = 0; i < 64; i++) {
        // Check retrieval
        assert(quadtree_query(ref, random[i], random[i]));
    }

    return 0;
}