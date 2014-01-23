#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "bitvec.h"

#define BOFF_TO_CELL_CEIL(off) (off + (sizeof(uint64_t) * 8))/(sizeof(uint64_t)*8)
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
    old_size = b->size;
    new_size = BOFF_TO_CELL_CEIL(to);
    if (old_size == new_size) return 1;
    // Reallocate the array
    b->storage = realloc(b->storage, sizeof(uint64_t)*new_size);
    if (b->storage == NULL) {
        fprintf(stderr, "bitvec_realloc_storage: realloc error%s\n", strerror(errno));
        exit(2);
    }
    // Clear the newly allocated memory
    for (i = old_size; i < new_size; i++) {
        b->storage[i] = 0;
    }
    b->max_offset = to;
    b->size = new_size;
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
    (*b)->size = 0;
    bitvec_realloc_storage(*b, size);
    return 0;
}

inline int bitvec_check(bitvec_t *b, uint64_t off) {
    uint64_t cell, bit, ret;
    bitvec_lock(b);

    if (off >= b->max_offset) {
        bitvec_realloc_storage(b, off);
    }

    cell = BOFF_TO_CELL(off);
    bit = BOFF_TO_BIT(off);

    ret = b->storage[cell] & ((uint64_t)1 << bit);
    bitvec_unlock(b);
    if (ret) {
        return 1;
    }
    return 0;
}

inline void bitvec_set(bitvec_t *b, uint64_t off) {

    uint64_t cell, bit;
    bitvec_lock(b);
    if (off >= b->max_offset) {
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

    if (off >= b->max_offset) {
        bitvec_realloc_storage(b, off);
    }

    cell = BOFF_TO_CELL(off);
    bit = BOFF_TO_BIT(off);

    b->storage[cell] &= ~((uint64_t)1 << bit);
    bitvec_unlock(b);
}

void bitvec_batch_set_u32(bitvec_t *b, uint32_t *labels, uint32_t count) {
    for (uint64_t i = 0; i < count; i++) {
        uint32_t label = *(labels + i);
        bitvec_set(b, label);
    }
}

void bitvec_clear_all(bitvec_t *b) {
    uint64_t max_cell = b->size;
    memset(b->storage, 0, max_cell * sizeof(uint64_t));
    b->max_offset = max_cell * sizeof(uint64_t) * 8;
}

void bitvec_union(bitvec_t *a, bitvec_t *b) {
    uint64_t i, m;

    // Find the minimum offset
    m = a->max_offset;
    if (b->max_offset < m) m = b->max_offset;

    m = BOFF_TO_CELL(m);
    for (i = 0; i < m; i++) {
        a->storage[i] = a->storage[i] | b->storage[i];
    }
}

void bitvec_free(bitvec_t *ref) {
    free(ref->storage);
    pthread_mutex_destroy(&(ref->lock));
    ref->storage = NULL;
    ref->max_offset = 0;
}

double bitvec_distance(bitvec_t *a, bitvec_t *b) {

    uint64_t i, u, m, j;
    bitvec_lock(a); bitvec_lock(b);

    i = 0;
    u = 0;

    // Find the minimum offset
    m = a->max_offset;
    if (b->max_offset < m) m = b->max_offset;

    m = BOFF_TO_CELL(m);
    for (j = 0; j <= m; j++) {
        uint64_t b1, b2, i1, u1;
        b1 = a->storage[j];
        b2 = b->storage[j];
        i1 = b1 & b2;
        u1 = b1 | b2;
        i += __builtin_popcountl(i1);
        u += __builtin_popcountl(u1);
    }
    bitvec_unlock(a); bitvec_unlock(b);

    return 1.0 - (double)i/u;
}

uint64_t bitvec_popcount(bitvec_t *b) {
    uint64_t ret, m, i;
    bitvec_lock(b);

    m = BOFF_TO_CELL(b->max_offset);
    for (i = 0, ret = 0; i < m; i++) {
        ret += __builtin_popcountl(b->storage[i]);
    }

    bitvec_unlock(b);
    return ret;
}
