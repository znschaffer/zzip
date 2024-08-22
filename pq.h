#ifndef PQ_H
#define PQ_H
#include <stddef.h>

#define PARENT(x) ((x - 1) / 2)
#define LEFT(x) ((2 * x) + 1)
#define RIGHT(x) ((2 * x) + 2)

struct pq_node {
	size_t id;
	void *item;
};

typedef struct pq {
	size_t size;
	size_t capacity;
	void **data;
	int (*cmp)(const void *d1, const void *d2);
} pq;

struct pq *pq_init(int (*cmp)(const void *d1, const void *d2), size_t capacity);

void pq_push(pq *pq, const void *item);

void pq_heapify(pq *pq, size_t idx);

void *pq_pop(pq *pq);

#endif // !PQ_H
