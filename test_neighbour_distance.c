#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

float neighbours_distance (
    unsigned int *a,
    unsigned int sa,
    unsigned int *b,
    unsigned int sb
    );

int main(int argc, char **argv) {
	unsigned int p0[] = {1};
	unsigned int p1[] = {3, 1};
	unsigned int p2[] = {3, 0};
	unsigned int p3[] = {0};
	unsigned int p4[] = {2};
	unsigned int p5[] = {4, 2};

	assert(neighbours_distance(p0, 1, p0, 1) == 0.0f);
	assert(neighbours_distance(p0, 1, p1, 2) == 0.5f);
	assert(neighbours_distance(p0, 1, p2, 2) == 1.0f);
	assert(neighbours_distance(p0, 1, p3, 1) == 1.0f);
	assert(neighbours_distance(p0, 1, p4, 1) == 1.0f);
	assert(neighbours_distance(p0, 1, p5, 2) == 1.0f);

	assert(fabs(neighbours_distance(p1, 2, p2, 2) - 0.6667f) < 0.01);
	assert(fabs(neighbours_distance(p2, 2, p3, 1) - 0.5f) < 0.01);
	assert(neighbours_distance(p3, 1, p4, 1) == 1.0f);
	assert(neighbours_distance(p4, 1, p5, 2) == 0.5f);

}