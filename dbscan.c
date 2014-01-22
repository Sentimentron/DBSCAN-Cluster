#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitvec.h"

// Data is expected to be in the form of identifiers which
// monotonically increase from 0 to whatever

// Maintain a bit array of what's a neighbour and what's not

int DBSCAN(void *data, unsigned int *d, unsigned int dlen,
            float eps, unsigned int minpoints,
            unsigned int (*neighbours_search)(bitvec_t *out,
            void *, unsigned int, float, unsigned int *)
    ) {

    unsigned int cluster = 1;
    unsigned int count, i, j, k;

    bitvec_t *visited, *clustered, *neighbours, *neighbours2;

    bitvec_alloc(&visited, dlen);
    bitvec_alloc(&clustered, dlen);
    bitvec_alloc(&neighbours, dlen);
    bitvec_alloc(&neighbours2, dlen);

    // d is a list of identifiers
    for (i = 0; i < dlen; i++) {
        count = 0;

        // Already visited this point
        if (bitvec_check(visited, i)) continue;

        // Mark this point as visited
        bitvec_set(visited, i);

        bitvec_clear_all(neighbours);

        // Get the first set of neighbours
        if(neighbours_search(neighbours, data, i, eps, &count)) {
            return 1;
        }
        if (count < minpoints) {
            *(d + i) = 0; // Noise
            continue;
        }

        *(d + i) = cluster;
        bitvec_set(clustered, i);

        // Expand the cluster
        for (j = 0; j < dlen; j++) {
            if(!bitvec_check(neighbours, j)) continue;
            if(!bitvec_check(visited, j)) {
                bitvec_set(visited, j);
                count = 0;
                bitvec_clear_all(neighbours2);
                if (neighbours_search(neighbours2, data, j, eps, &count)) {
                    return 1;
                }
                if (count >= minpoints) {
                    // Merge two bitarrays
                    bitvec_union(neighbours, neighbours2);
                }
            }
            if (!bitvec_check(clustered, j)) {
                *(d + j) = cluster;
                bitvec_set(clustered, j);
            }
        }

        cluster++;
    }

    bitvec_free(visited);
    bitvec_free(clustered);
    bitvec_free(neighbours);
    bitvec_free(neighbours2);

    return 0;

}