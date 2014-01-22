#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITVEC_ALLOC(bitvec, size) bitvec = calloc(1, (size+7)/8); \
    if(bitvec == NULL) {\
        fprintf(stderr, "%s:%d: Allocation error!\n", __FILE__, __LINE__); \
        exit(2); \
    }
#define BITVEC_INS(off, bitvec) bitvec[off / 8] |= (1 << (off % 8))
#define BITVEC_SET(off, bitvec) bitvec[off / 8] & (1 << (off % 8))

// Data is expected to be in the form of identifiers which
// monotonically increase from 0 to whatever

// Maintain a bit array of what's a neighbour and what's not

int DBSCAN(void *data, unsigned int *d, unsigned int dlen,
            float eps, unsigned int minpoints,
            unsigned int (*neighbours_search)(char *out,
            void *, unsigned int, float, unsigned int, unsigned int *)
    ) {

    unsigned int cluster = 1;
    char *visited, *clustered; // Bit-vector which says if we've visited an offset in D
    char *neighbours, *neighbours2;
    unsigned int count, i, j, k;


    BITVEC_ALLOC(visited, dlen);
    BITVEC_ALLOC(clustered, dlen);
    BITVEC_ALLOC(neighbours, dlen);
    BITVEC_ALLOC(neighbours2, dlen);

    // d is a list of identifiers
    for (i = 0; i < dlen; i++) {
        // Already visited this point
        if (BITVEC_SET(i, visited)) continue;

        // Mark this point as visited
        BITVEC_INS(i, visited);

        memset(neighbours, 0, (dlen+7)/8);
        // Get the first set of neighbours
        if(neighbours_search(neighbours, data, i, eps, dlen, &count)) {
            return 1;
        }

        if (count < minpoints) {
            *(d + i) = 0; // Noise
            continue;
        }

        *(d + i) = cluster;
        BITVEC_INS(i, clustered);

        // Expand the cluster
        for (j = 0; j < dlen; j++) {
            if(!(BITVEC_SET(j, neighbours))) continue;
            if(!(BITVEC_SET(j, visited))) {
                BITVEC_INS(j, visited);
                memset(neighbours2, 0, (dlen+7)/8);
                if (neighbours_search(neighbours2, data, j, eps, dlen, &count)) {
                    return 1;
                }
                if (count >= minpoints) {
                    // Merge two bitarrays
                    for (k = 0; k < (dlen + 7)/8; k++) {
                        neighbours[k] |= neighbours2[k];
                    }
                }
            }
            if (!(BITVEC_SET(j, clustered))) {
                *(d + j) = cluster;
                BITVEC_INS(j, clustered);
            }
        }

        cluster++;
    }

    free(visited);
    free(clustered);
    free(neighbours);
    free(neighbours2);

    return 0;

}