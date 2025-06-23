
 #include <assert.h> /*assert*/
 #include <stdlib.h> /* malloc, free*/
 #include <unistd.h> /*sleep*/

 #include "task.h"
 
struct task
{
    ilrd_uid_t uid;
    int (*action_func)(void* params);
    void* params;
    size_t interval_in_sec;
    time_t time_to_run;
};

task_t* TaskCreate(int (*action_func)(void* params), void* params,
					size_t interval_in_sec)
{
	task_t* task = NULL;
	
	assert(action_func);
	task =(task_t*)malloc(sizeof(task_t));
	
	if (NULL == task)
	{
		return task;
	}
	
	task->uid = UIDCreate();
	
	if (UIDIsSame(task->uid, bad_uid))
	{
		free(task);
		return NULL;
	}
	
	task->action_func = action_func;
	task->params = params;
	task->interval_in_sec = interval_in_sec;
	task->time_to_run = time(NULL) + interval_in_sec;
	
	return task;
}

void TaskDestroy(task_t* task)
{
	assert(task);
	free(task);
}

int TaskRun(task_t* task)
{
	assert(task);
	

	while (task->time_to_run > time(NULL))
	{
		sleep(task->time_to_run - time(NULL));
	}
	
	return (task->action_func(task->params));
}

ilrd_uid_t TaskGetUID(const task_t* task)
{
	assert(task);
	return task->uid;
}

size_t TaskGetInterval(const task_t* task)
{
	assert(task);
	return task->interval_in_sec;
}

void TaskSetTimeToRun(task_t* task)
{
	assert(task);
	task->time_to_run = task->interval_in_sec + time(NULL);
}

time_t TaskGetTimeToRun(const task_t* task)
{
	assert(task);
	return (task->time_to_run);
}

int TaskIsEqual(const task_t* task1, const task_t* task2)
{
	assert(task1);
	assert(task2);
	
	return (task1 == task2);
}
