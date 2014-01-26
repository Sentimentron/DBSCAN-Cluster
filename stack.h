#ifndef __STACK_H
#define __STACK_H

#include <stdint.h>

typedef struct {
	unsigned int size;
	unsigned int storing;
	void **storage;
} linear_stack_t;

void stack_init(linear_stack_t *s);
void stack_realloc(linear_stack_t *s, unsigned int size);

static inline int stack_push(linear_stack_t *s, void *p) {

	if(s->storing > s->size-1) {
		stack_realloc(s, s->size * 2);
	}

	*(s->storage + s->storing++) = p;
	return 0;
}

static inline void* stack_pop(linear_stack_t *s) {
	void *ret;
	if (s->storing == 0) return NULL;
	ret = *(s->storage + (s->storing-1));
	s->storing--;
	return ret;
}

#endif
