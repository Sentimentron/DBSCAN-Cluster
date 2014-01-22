#include <assert.h>
#include "bitvec.h"

int main(int argc, char **argv) {
    bitvec_t *b = NULL;

    assert(!bitvec_alloc(&b, 1));

    bitvec_set(b, 0);
    assert(b->storage[0] == 0x1);
    assert(bitvec_check(b, 0));

    bitvec_set(b, 63);
    assert(b->storage[0] == ((1ul << 63) | 0x1));
    assert(bitvec_check(b, 0));
    assert(bitvec_check(b, 63));

    return 0;
}