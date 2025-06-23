
#include <assert.h> /*assert*/
#include <stdlib.h> /*malloc, free*/

#include "p_queue_heap.h"
#include "heap.h"

struct priority_queue
{
	heap_t* heap;
};

pq_t* PQCreate(int (*compare_func)(const void*, const void*))
{
	pq_t* pq = NULL;

	assert(compare_func);
	pq = (pq_t*)malloc(sizeof(pq_t));
	if (!pq)
	{
		return NULL;
	}
	pq->heap = HeapCreate(compare_func);
	if (!pq->heap)
	{
		free(pq);
		return NULL;
	}
	
	return pq;
}

void PQDestroy(pq_t* pq)
{
	assert(pq);
    HeapDestroy(pq->heap);
	free(pq);
}

int PQEnqueue(pq_t* pq, void* data)
{
	assert(pq);
    return HeapPush(pq->heap, data);
}

void* PQDequeue(pq_t* pq)
{
    void* data = NULL;

	assert(pq);
	assert(!PQIsEmpty(pq));
    data = HeapPeek(pq->heap);
    HeapPop(pq->heap);

	return data;
}

void* PQPeek(const pq_t* pq)
{
	assert(pq);
	assert(!PQIsEmpty(pq));
	return HeapPeek(pq->heap);
}

int PQIsEmpty(const pq_t* pq)
{
	assert(pq);
	return HeapIsEmpty(pq->heap);
}

size_t PQSize(const pq_t* pq)
{
	assert(pq);
    return HeapSize(pq->heap);	
}

void PQClear(pq_t* pq)
{
	assert(pq);
	while (!PQIsEmpty(pq))
	{
		PQDequeue(pq);
	}
}

void* PQErase(pq_t* pq, int (*is_match)(const void*, const void*),
				const void* param)
{
	assert(pq);
	assert(!PQIsEmpty(pq));
	assert(is_match);

	return HeapRemove(pq->heap, (void*)param, is_match);
}
