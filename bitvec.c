#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "bitvec.h"

#define BOFF_TO_CELL_CEIL(off) (off + (sizeof(uint64_t) * 8) - 1)/(sizeof(uint64_t)*8)
#define BOFF_TO_CELL(off) (off / (sizeof(uint64_t) * 8))
#define BOFF_TO_BIT(off)  (off % (sizeof(uint64_t) * 8))

void bitvec_lock(bitvec_t *b) {
    int status = pthread_mutex_lock(&(b->lock));
    if (status) {
        fprintf(stderr, "bv_lock: %s\n", strerror(errno));
        exit(3);
    }
}

void bitvec_unlock(bitvec_t *b) {
    int status = pthread_mutex_unlock(&(b->lock));
    if (status) {
        fprintf(stderr, "bv_unlock: %s\n", strerror(errno));
        exit(3);
    }
}

int bitvec_realloc_storage(bitvec_t *b, uint64_t to) {
    uint64_t old_size, new_size, i;
    // Check parameters
    if (to < b->max_offset) return 1;
    // Figure out size in uint64_ts
    old_size = BOFF_TO_CELL_CEIL(b->max_offset);
    new_size = BOFF_TO_CELL_CEIL(to);
    // Reallocate the array
    b->storage = realloc(b->storage, sizeof(uint64_t)*new_size);
    if (b->storage == NULL) {
        fprintf(stderr, "bitvec_realloc_storage: realloc error%s\n", strerror(errno));
        exit(2);
    }
    // Clear the newly allocated memory
    for (i = old_size+1; i < new_size; i++) {
        b->storage[i] = 0;
    }
    b->max_offset = to;
    return 0;
}

int bitvec_alloc(bitvec_t **b, uint64_t size) {
    int s;
    // Check the parameters
    if (b == NULL) return 1;
    if (!size) return 1;
    // Allocate the vector
    *b = malloc(sizeof(bitvec_t));
    if (*b == NULL) {
        fprintf(stderr, "bv_alloc: Allocation error %s\n", strerror(errno));
        exit(2);
    }
    // Initialize mutex
    s = pthread_mutex_init(&((*b)->lock), NULL);
    if (s) {
        fprintf(stderr, "bv_alloc: Mutex initialization error %s\n", strerror(errno));
        exit(2);
    }
    // Resize the right size
    (*b)->storage = NULL;
    (*b)->max_offset = 0;
    bitvec_realloc_storage(*b, size);
    return 0;
}

inline int bitvec_check(bitvec_t *b, uint64_t off) {
    uint64_t cell, bit, ret;
    bitvec_lock(b);

    if (off > b->max_offset) {
        bitvec_realloc_storage(b, off);
    }

    cell = BOFF_TO_CELL(off);
    bit = BOFF_TO_BIT(off);

    ret = b->storage[cell] &= ((uint64_t)1 << bit);
    bitvec_unlock(b);
    return ret;
}

inline void bitvec_set(bitvec_t *b, uint64_t off) {

    uint64_t cell, bit;
    bitvec_lock(b);
    if (off > b->max_offset) {
        bitvec_realloc_storage(b, off);
    }

    cell = BOFF_TO_CELL(off);
    bit = BOFF_TO_BIT(off);

    b->storage[cell] |= ((uint64_t)1 << bit);
    bitvec_unlock(b);
}

inline void bitvec_clear(bitvec_t *b, uint64_t off) {
 
    uint64_t cell, bit;
    bitvec_lock(b);

    if (off > b->max_offset) {
        bitvec_realloc_storage(b, off);
    }

    cell = BOFF_TO_CELL(off);
    bit = BOFF_TO_BIT(off);

    b->storage[cell] &= ~((uint64_t)1 << bit);
    bitvec_unlock(b);
}