#include <assert.h>
#include "bitvec.h"

int main(int argc, char **argv) {
    
    bitvec_t *b = NULL;
    assert(!bitvec_alloc(&b, 1));

    bitvec_set(b, 0);
    assert(b->storage[0] == 0x1);

    bitvec_set(b, 63);
    assert(b->storage[0] == ((1ul << 63) | (1ul)));

    bitvec_set(b, 64);
    assert(b->storage[0] == ((1ul << 63) | (1ul)));
    assert(b->storage[1] == 0x1);

    return 0;
}