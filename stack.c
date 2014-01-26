#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

void stack_init(linear_stack_t *s) {
	s->storing = 0;
	s->size = 0;
	s->storage = NULL;
}

void stack_realloc(linear_stack_t *s, unsigned int size) {
	s->storage = realloc(s->storage, size * sizeof(void *));
	if (size && s->storage == NULL) {
		fprintf(stderr, "Allocation error!\n");
		exit(2);
	}
	s->size = size;
}

/*inline int stack_push(linear_stack_t *s, uint64_t p) {

	if(s->storing > s->size-1) {
		stack_realloc(s, s->size * 2);
	}

	*(s->storage + s->storing++) = p;
	return 0;
}

inline uint64_t stack_pop(linear_stack_t *s) {
	uint64_t ret;
	if (s->storing == 0) return NULL;
	ret = *(s->storage + (s->storing-1));
	s->storing--;
	return ret;
}*/
