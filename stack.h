#ifndef __STACK_H
#define __STACK_H

#include <stdint.h>

typedef struct {
	unsigned int size;
	unsigned int storing;
	uint64_t *storage;
} linear_stack_t;

void stack_init(linear_stack_t *s);
void stack_realloc(linear_stack_t *s, unsigned int size);
int stack_push(linear_stack_t *s, uint64_t p);
uint64_t stack_pop(linear_stack_t *s);

#endif
