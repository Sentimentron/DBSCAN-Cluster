#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack.h"

int main (int argc, char **argv) {
	linear_stack_t s;

	stack_init(&s);
	assert(s.storing == 0);
	assert(s.size == 0);
	assert(s.storage == NULL);

	stack_realloc(&s, 1);
	assert(s.storing == 0);
	assert(s.size == 1);
	assert(s.storage != NULL);

	stack_push(&s, 1);
	stack_push(&s, 2);

	assert(stack_pop(&s) == 2);
	assert(stack_pop(&s) == 1);

	stack_realloc(&s, 0);

}
