#ifndef _H_BITVEC
#define _H_BITVEC

#include <stdint.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    uint64_t        max_offset;
    uint64_t        *storage;
    size_t          size;
} bitvec_t; 

int bitvec_check(bitvec_t*, uint64_t);
void bitvec_set(bitvec_t *, uint64_t);
void bitvec_clear(bitvec_t *, uint64_t);
int bitvec_alloc(bitvec_t **, uint64_t);
void bitvec_batch_set_u32(bitvec_t *, uint32_t *, uint32_t);
void bitvec_clear_all(bitvec_t *);
void bitvec_union(bitvec_t*, bitvec_t*);
void bitvec_free(bitvec_t*);
double bitvec_distance(bitvec_t*, bitvec_t*);
uint64_t bitvec_popcount(bitvec_t *b);

#endif