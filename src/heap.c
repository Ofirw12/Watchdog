
#include <assert.h> /*assert*/
#include <stdlib.h> /*malloc, free*/

#include "heap.h"
#include "dvector.h"

#define CAPACITY (32)
#define PARENT_INDEX(i) ((i - 1) / 2)
#define LEFT_INDEX(i) ((2 * i) + 1)
#define RIGHT_INDEX(i) ((2 * i) + 2)

struct heap 
{ 
    dvector_t* vector; 
    compare_func_t compare_func; 
};

static void SwapIndexes(dvector_t* vector, size_t one_index, size_t other_index)
{
    void* one = NULL;
    void* other = NULL;

    assert(vector);

    DvectorGetElement(vector, one_index, &one);
    DvectorGetElement(vector, other_index, &other);
    DvectorSetElement(vector, one_index, &other);
    DvectorSetElement(vector, other_index, &one);
}

static void HeapifyUp(heap_t* heap, size_t i)
{
    void* parent = NULL;
    void* child = NULL;

    assert(heap);

    if (0 == i)
    {
        return;
    }

    DvectorGetElement(heap->vector, i, &child);
    DvectorGetElement(heap->vector, PARENT_INDEX(i), &parent);

    if (heap->compare_func(child, parent) < 0)
    {
        SwapIndexes(heap->vector, i, PARENT_INDEX(i));
        HeapifyUp(heap, PARENT_INDEX(i));
    }
}

static void HeapifyDown(heap_t* heap, size_t i)
{
    void* parent = NULL;
    void* left = NULL;
    void* right = NULL;
    void* min_child = NULL;
    size_t child_index = 0;

    assert(heap);

    if ((i >= HeapSize(heap)) || (LEFT_INDEX(i) >=  HeapSize(heap)))
    {
        return;
    }

    DvectorGetElement(heap->vector, i, &parent);
    DvectorGetElement(heap->vector, LEFT_INDEX(i), &left);

    if ((RIGHT_INDEX(i) < HeapSize(heap)))
    {
        DvectorGetElement(heap->vector, RIGHT_INDEX(i), &right);
        min_child = (heap->compare_func(left, right) < 0) ? left : right;
        child_index = (min_child == left) ? LEFT_INDEX(i) : RIGHT_INDEX(i);

        if (heap->compare_func(min_child, parent) < 0)
        {
            SwapIndexes(heap->vector, i, child_index);
            HeapifyDown(heap, child_index);
        }
        return;
    }

    if (heap->compare_func(left, parent) < 0)
    {
        SwapIndexes(heap->vector, i, LEFT_INDEX(i));
        HeapifyDown(heap, LEFT_INDEX(i));
    }
}

static void* FindAndRemove(heap_t* heap, const void* param, 
                            is_match_t is_match, size_t index)
{
    void* data = NULL;
    assert(heap);
    assert(is_match);

    if (index >= HeapSize(heap))
    {
        return NULL;
    }

    DvectorGetElement(heap->vector, index, &data);

    if (is_match(data, param))
    {
        SwapIndexes(heap->vector, index, HeapSize(heap) - 1);
        DvectorPopBack(heap->vector);
        HeapifyDown(heap, index);
        return data;
    }

    data = FindAndRemove(heap, param, is_match, LEFT_INDEX(index));

    if (NULL != data)
    {
        return data;
    }

    return (FindAndRemove(heap, param, is_match, RIGHT_INDEX(index)));
}
 

heap_t* HeapCreate(compare_func_t compare_func)
{
    heap_t* heap = NULL;

    assert(compare_func);

    heap = (heap_t*)malloc(sizeof(heap_t));

    if (NULL == heap)
    {
        return heap;
    }
    heap->compare_func = compare_func;
    heap->vector = DvectorCreate(CAPACITY, sizeof(void*));
    
    if (NULL == heap->vector)
    {
        free(heap);
        return NULL;
    }

    return heap;
}
 
void HeapDestroy(heap_t* heap)
{
    assert(heap);
    DvectorDestroy(heap->vector);
    free(heap);
}
 
int HeapPush(heap_t* heap, void* data)
{
    assert(heap);

    if (1 == DvectorPushBack(heap->vector, &data))
    {
        return 1;
    }
    HeapifyUp(heap, (DvectorSize(heap->vector) - 1));

    return 0;
}
 
int HeapPop(heap_t* heap)
{
    int res = 0;
    assert(heap);
    assert(!HeapIsEmpty(heap));

    SwapIndexes(heap->vector, 0, (DvectorSize(heap->vector) - 1));
    res = DvectorPopBack(heap->vector);
    HeapifyDown(heap, 0);
    return res;
}
 
void* HeapPeek(const heap_t *heap)
{
    void* data = NULL;
    assert(heap);
    DvectorGetElement(heap->vector, 0, &data);

    return data;
}
 
size_t HeapSize(const heap_t* heap)
{
    assert(heap);
    return DvectorSize(heap->vector);
}
 
int HeapIsEmpty(const heap_t *heap)
{
    assert(heap);
    return (0 == DvectorSize(heap->vector));
}

void* HeapRemove(heap_t* heap, const void* param, is_match_t is_match)
{
    assert(heap);
    assert(is_match);

    return FindAndRemove(heap, param, is_match, 0);
}