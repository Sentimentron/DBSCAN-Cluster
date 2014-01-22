#include <assert.h>
#include "bitvec.h"

int main(int argc, char **argv) {
    bitvec_t *b = NULL;
    assert(bitvec_alloc(&b, 0) == 1);
    assert(bitvec_alloc(NULL,1) == 1);
    assert(!bitvec_alloc(&b, 1));
    return 0;
}