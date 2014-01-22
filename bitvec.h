#ifndef _H_BITVEC
#define _H_BITVEC

#include <stdint.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    uint64_t        max_offset;
    uint64_t        *storage;
} bitvec_t; 

int bitvec_check(bitvec_t*, uint64_t);
void bitvec_set(bitvec_t *, uint64_t);
void bitvec_clear(bitvec_t *, uint64_t);
int bitvec_alloc(bitvec_t **, uint64_t);

#endif