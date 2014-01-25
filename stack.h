#ifndef __STACK_H
#define __STACK_H

typedef struct {
	unsigned int size,
	unsigned int storage,
	void *storage
} stack_t;

void stack_init(stack_t *s);
void stack_realloc(stack_t *s, unsigned int size);
int stack_push(stack_t *s, void *p);
void *stack_pop(stack_t *s);

#endif
