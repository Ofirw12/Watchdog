#include "scheduler_heap.h" 
#include "test_macros.h"

/*int (*action_func)(void* params)*/
static int add_values[3] = {1,2,0};
static int divide_values[3] = {200,100,0};
static int multiply_values[3] = {1,2,0};

static int AddNums(void* params)
{
	int* a = (int*)params;
	int* b = (int*)params + 1;
	int* res = (int*)params + 2;
	
	if(NULL == a || NULL == b || NULL == res)
	{
		return 0;
	}
	
	*res = *a + *b;
	return 0;
}

static int DivideNums(void* params)
{
	int* a = (int*)params;
	int* b = (int*)params + 1;
	int* res = (int*)params + 2;
	
	if(NULL == a || NULL == b || NULL == res)
	{
		return 0;
	}
	
	*res = *a / *b;
	return 0;
}

static int MultiplyNums(void* params)
{
	int* a = (int*)params;
	int* b = (int*)params + 1;
	int* res = (int*)params + 2;
	
	if(NULL == a || NULL == b || NULL == res)
	{
		return 0;
	}
	
	*res = *a * *b;
	return 0;
}

static int StopSchedulerFunc(void* params)
{
	scheduler_t* sched = (scheduler_t*)params;
	
	SchedulerStop(sched);
	
	return 1;
}

static int AddSchedulerFunc(void* params)
{
	static int counter = 0;
	scheduler_t* sch = (scheduler_t*)params;
	SchedulerAdd(sch, MultiplyNums, &multiply_values, 3);
	
	return (5 == counter++);
}


static int DestroySchedulerFunc(void* params)
{
	scheduler_t* sched = (scheduler_t*)params;
	
	SchedulerDestroy(sched);
	
	return 1;
}

static int RunSchedulerFunc(void* params)
{
	scheduler_t* sched = (scheduler_t*)params;
	
	return ((NULL == sched) ? 1 : SchedulerRun(sched));
}

static int OneTimeFunc(void* params)
{
	(void)params;
	return 1;
}

void TestScheduler(void)
{
	scheduler_t* sch = SchedulerCreate();
	scheduler_t* sch2 = SchedulerCreate();
	ilrd_uid_t identifiers[5]= {0};
	status_t stat = 0;
	
	identifiers[0] = SchedulerAdd(sch, AddNums, &add_values, 2);
	identifiers[1] = SchedulerAdd(sch, DivideNums, &divide_values, 4);
	identifiers[2] = SchedulerAdd(sch, MultiplyNums, &multiply_values, 3);
	identifiers[3] = SchedulerAdd(sch, StopSchedulerFunc, sch, 6);
	identifiers[4] = SchedulerAdd(sch, OneTimeFunc, NULL, 5);
	
	TEST("Scheduler size test", SchedulerSize(sch), 5);
	stat = SchedulerRun(sch);
	TEST("Scheduler Stopped", stat, STOPPED);
	TEST("Scheduler size test after some tasks finished", SchedulerSize(sch), 3);
	SchedulerRemove(sch, identifiers[2]);
	TEST("Scheduler size test after 1 task removed", SchedulerSize(sch), 2);
	SchedulerClear(sch);
	TEST("Scheduler is empty", SchedulerIsEmpty(sch), 1);
	
	identifiers[0] = SchedulerAdd(sch, OneTimeFunc, NULL, 1);
	stat = SchedulerRun(sch);
	TEST("Scheduler finished running", stat, SUCCESS);
	TEST("Scheduler is empty", SchedulerIsEmpty(sch), 1);
	
	identifiers[0] = SchedulerAdd(sch, AddSchedulerFunc, sch2, 2);
	SchedulerRun(sch);
	TEST("Scheduler2 size test after adds", SchedulerSize(sch2), 6);
	
	SchedulerAdd(sch2, DestroySchedulerFunc, sch2, 2);
	
	identifiers[0] = SchedulerAdd(sch, RunSchedulerFunc, sch2, 1);
	stat = SchedulerRun(sch);
	
	identifiers[1] = SchedulerAdd(sch, RunSchedulerFunc, sch, 1);
	TEST("Scheduler size test after add", SchedulerSize(sch), 1);
	stat = SchedulerRun(sch);
	TEST("Scheduler status while running test", stat, SUCCESS);
	SchedulerDestroy(sch);
}

int main(void)
{
	TestScheduler();
	PASS;
	return 0;
}

