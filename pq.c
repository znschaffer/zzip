#include "pq.h"
#include <stdlib.h>

struct pq *pq_init(int (*cmp)(const void *d1, const void *d2), size_t capacity)
{
	pq *self = NULL;
	self = malloc(sizeof(*self));
	self->cmp = cmp;
	self->data = malloc(capacity * sizeof(*(self->data)));
	self->size = 0;
	self->capacity = capacity;
	return self;
}

void pq_push(pq *pq, const void *item)
{
	size_t i;
	void *tmp = NULL;

	pq->data[pq->size] = (void *)item;
	i = pq->size;
	pq->size++;

	while (i > 0 && pq->cmp(pq->data[i], pq->data[PARENT(i)]) < 0) {
		tmp = pq->data[i];
		pq->data[i] = pq->data[PARENT(i)];
		pq->data[PARENT(i)] = tmp;
		i = PARENT(i);
	}
}

void pq_heapify(pq *pq, size_t idx)
{
	void *tmp = NULL;

	size_t l_idx, r_idx, smallest_idx;

	l_idx = LEFT(idx);
	r_idx = RIGHT(idx);

	/* Left child exists, compare left child with its parent */
	if (l_idx < pq->size && pq->cmp(pq->data[l_idx], pq->data[idx]) < 0) {
		smallest_idx = l_idx;
	} else {
		smallest_idx = idx;
	}

	/* Right child exists, compare right child with the largest element */
	if (r_idx < pq->size &&
	    pq->cmp(pq->data[r_idx], pq->data[smallest_idx]) < 0) {
		smallest_idx = r_idx;
	}

	/* At this point largest element was determined */
	if (smallest_idx != idx) {
		/* Swap between the index at the largest element */
		tmp = pq->data[smallest_idx];
		pq->data[smallest_idx] = pq->data[idx];
		pq->data[idx] = tmp;

		/* Heapify again */
		pq_heapify(pq, smallest_idx);
	}
}

void *pq_pop(pq *pq)
{
	if (pq->size <= 0) {
		return NULL;
	}
	void *node = pq->data[0];

	pq->data[0] = pq->data[--pq->size];

	pq_heapify(pq, 0);

	return node;
}
