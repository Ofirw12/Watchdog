
#include <assert.h> /*assert*/
#include <stdlib.h> /* malloc, free*/

#include "scheduler_heap.h" 
#include "p_queue_heap.h"
#include "task.h" /*Task*/


struct scheduler
{
	pq_t* queue;
	status_t status;
};

static int CompareFunc(const void* one, const void* other)
{
	return (TaskGetTimeToRun((task_t*)one) - TaskGetTimeToRun((task_t*)other));
}

static int TaskUIDIsSame(const void* task, const void* uid)
{
	assert(task);
	assert(uid);
	return (UIDIsSame(TaskGetUID((task_t*)task), *(ilrd_uid_t*)uid));
}

static int TaskHandler(scheduler_t* scheduler, task_t* task)
{
	assert(scheduler);
	assert(task);
	
	if (0 != TaskRun(task))
	{
		TaskDestroy(task);
	}
	else
	{
		TaskSetTimeToRun(task);
		if (0 != PQEnqueue(scheduler->queue, task))
		{
			TaskDestroy(task);
			return ERROR;
		}
	}
	return SUCCESS;
}

scheduler_t* SchedulerCreate(void)
{
	scheduler_t* sch = (scheduler_t*)malloc(sizeof(scheduler_t));
	
	if (NULL == sch)
	{
		return sch;
	}
	sch->queue = PQCreate(CompareFunc);
	
	if (NULL == sch->queue)
	{
		free(sch);
		return NULL;
	}
	sch->status = STOPPED;
	
	return sch;
}

void SchedulerDestroy(scheduler_t* scheduler)
{
	assert(scheduler);
	
	if (RUNNING == scheduler->status)
	{
		scheduler->status = DESTROYED;
		return;
	}
	
	SchedulerClear(scheduler);
	PQDestroy(scheduler->queue);
	free(scheduler);
}

ilrd_uid_t SchedulerAdd(scheduler_t* scheduler,
						int (*action_func)(void* params),
						void* params, size_t interval_in_sec)
{
	task_t* new_task = NULL;

	assert(scheduler);
	assert(action_func);
	
	new_task = TaskCreate(action_func, params, interval_in_sec);
	if ( NULL == new_task)
	{
		return bad_uid;
	}
	if (0 != PQEnqueue(scheduler->queue, new_task))
	{
		TaskDestroy(new_task);
		return bad_uid;
	}
	
	return TaskGetUID(new_task);
}

int SchedulerRemove(scheduler_t* scheduler, ilrd_uid_t identifier)
{
	task_t* to_remove = NULL;
	
	assert(scheduler);
	assert(!UIDIsSame(identifier, bad_uid));
	
	if(RUNNING == scheduler->status)
	{
		return 1;
	}

	to_remove = ((task_t*)(PQErase(scheduler->queue,
				TaskUIDIsSame, &identifier)));
	if (NULL == to_remove)
	{
		return 1;
	}
	TaskDestroy(to_remove);
	
	return 0;
}

status_t SchedulerRun(scheduler_t* scheduler)
{
	task_t* task = NULL;	
	
	assert(scheduler);
	if (RUNNING == scheduler->status)
	{
		return RUNNING;
	}
	scheduler->status = RUNNING;
	
	while(RUNNING == scheduler->status && !SchedulerIsEmpty(scheduler))
	{
		task = (task_t*)PQDequeue(scheduler->queue);
		if (0 != TaskHandler(scheduler, task))
		{
			return ERROR;
		}
	}
	
	if (DESTROYED == scheduler->status)
	{
		SchedulerDestroy(scheduler);
		return DESTROYED;
	}
	
	if (STOPPED == scheduler->status)
	{
		return STOPPED;
	}
	
	scheduler->status = STOPPED;
	return SUCCESS;
}

void SchedulerStop(scheduler_t* scheduler)
{
	assert(scheduler);
	scheduler->status = STOPPED;
}

size_t SchedulerSize(const scheduler_t* scheduler)
{
	assert(scheduler);
	return PQSize(scheduler->queue);
}

int SchedulerIsEmpty(const scheduler_t* scheduler)
{
	assert(scheduler);
	return PQIsEmpty(scheduler->queue);
}

void SchedulerClear(scheduler_t* scheduler)
{
	assert(scheduler);
	while(!SchedulerIsEmpty(scheduler))
	{
		TaskDestroy((task_t*)PQDequeue(scheduler->queue));
	}
}

