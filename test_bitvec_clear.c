#include <assert.h>
#include "bitvec.h"

int main(int argc, char **argv) {
    bitvec_t *b = NULL;
    assert(!bitvec_alloc(&b, 1));

    bitvec_set(b, 0);
    assert(b->storage[0] == 0x1);

    bitvec_clear(b, 0);
    assert(b->storage[0] == 0x0);

    bitvec_set(b, 63);
    assert(b->storage[0] == (1ul << 63));

    bitvec_clear(b, 63);
    assert(b->storage[0] == 0);

    return 0;
}