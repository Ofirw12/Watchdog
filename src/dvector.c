
#include <assert.h> /*	assert	*/
#include <stdlib.h> /*	malloc, realloc, free	*/
#include <string.h> /*	memcpy	*/

#include "dvector.h"

#define RESIZE_FACTOR 1.5f

struct dvector
{
	size_t capacity;
	size_t element_size;
	size_t size;
	void* arr;
}; 

dvector_t* DvectorCreate(size_t capacity, size_t element_size)
{
	dvector_t* vector = (dvector_t*)malloc(sizeof(dvector_t));
	if (NULL == vector)
	{
		return vector;
	}
	vector->capacity = capacity;
	vector->element_size = element_size;
	vector->size = 0;
	vector->arr = malloc(capacity * element_size);
	if (NULL == vector->arr)
	{
		free(vector);
		return NULL;
	}
	return vector;
}

void DvectorDestroy(dvector_t* dvector)
{
	free(dvector->arr);
	free(dvector);
}

size_t DvectorCapacity(const dvector_t* dvector)
{
	assert(NULL != dvector);
	return dvector->capacity;
}

size_t DvectorSize(const dvector_t* dvector)
{
	assert(NULL != dvector);
	return dvector->size;
}

int DvectorResize(dvector_t* dvector, size_t new_capacity)
{
	assert(NULL != dvector);
	
	if(dvector->capacity == new_capacity)
	{
		return 0;
	}
	dvector->arr = realloc(dvector->arr, (new_capacity * dvector->element_size));
	dvector->capacity = new_capacity;
	dvector->size = (dvector->size > dvector->capacity) ? dvector->capacity : dvector->size;
	
	return !(dvector->arr);
}

void DvectorGetElement(const dvector_t* dvector, size_t index, void* dest)
{
	unsigned char* arr;

	assert(NULL != dvector);
	assert(NULL != dest);
	assert(index < (dvector->size));

	arr = (unsigned char*) dvector->arr;
	
	memcpy(dest, arr + (index * dvector->element_size), dvector->element_size);
}

void DvectorSetElement(dvector_t* dvector, size_t index, const void* value)
{
	unsigned char* arr;

	assert(NULL != dvector);
	assert(NULL != value);
	assert(index < (dvector->size));

	arr = (unsigned char*) dvector->arr;
	memcpy(arr + (index * dvector->element_size), value, dvector->element_size);
}
int DvectorShrink(dvector_t* dvector)
{
	assert(NULL != dvector);
	return DvectorResize(dvector, dvector->size);
}
int DvectorPushBack(dvector_t* dvector, const void* element)
{
	size_t new_capacity = 0;
	
	assert(NULL != dvector);
	assert(NULL != element);

	new_capacity = (size_t)((RESIZE_FACTOR * (dvector->size)) + 1);

	if(dvector->size == dvector->capacity)
	{
		if (1 == DvectorResize(dvector, new_capacity))
		{
			return 1;
		}
	}
	++dvector->size;
	DvectorSetElement(dvector, dvector->size -1, element);
	return 0;
}

int DvectorPopBack(dvector_t* dvector)
{
	size_t test_new_capacity = 0;
	
	assert(NULL != dvector);
	assert(0 != dvector->size);

	test_new_capacity = (size_t)(dvector->capacity / RESIZE_FACTOR);

	if (dvector->size <= test_new_capacity)
	{
		if (1 == DvectorResize(dvector, test_new_capacity))
		{
			return 1;
		}
	}
	--dvector->size;
	return 0;
}




