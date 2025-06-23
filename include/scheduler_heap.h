#ifndef __SCHEDULER_HEAP_H__
#define __SCHEDULER_HEAP_H__

#include <stddef.h>     /* size_t */

#include "ilrd_uid.h"   /* ilrd_uid_t */

typedef struct scheduler scheduler_t;
 
typedef enum status
{
    SUCCESS = 0,
    STOPPED = 1,
    ERROR = 2,
    DESTROYED = 3,
    RUNNING = 4
} status_t;


/* 
*   @desc:          Allocates new scheduler must be destroyed with 
*					@SchedulerDestroy
*   @params: 		None
*   @return value:  Pointer to the new scheduler
*   @error: 		Returns NULL if allocation fails
*   @time complex: 	O(malloc) for both AC/WC
*   @space complex: O(malloc) for both AC/WC
*/
scheduler_t* SchedulerCreate(void);


/* 
*   @desc:          Destroys and frees @scheduler. In the event the scheduler is
*			    	still running it will signal to @scheduler to destroy
*			    	itself, after the current task is done running and will 
*			    	return DESTROYED on	@SchedulerRun return value
*   @params: 		@scheduler: pre allocated scheduler
*   @return value:  None
*   @error: 		Undefined behavior if @scheduler is not valid
*   @time complex: 	O(n) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
void SchedulerDestroy(scheduler_t* scheduler);


/* 
*   @desc:          Adds a new task to @scheduler that will perform @action_func
*				    with @params as params to @action_func and @interval_in_sec
*				    which will say the amount of time between each invocation of
*				    @action_func should pass
*   @params: 	    @scheduler: pre allocated scheduler
*				    @action_func: user function that the task will perform it
*			    	will return 0 if it should repeat or non-zero value to
*                   indicate it shouldn't repeat no more.
*		    		@params: user pointer to additional data the user might want
*		    		to send to the function.
*		    		@interval_sec: the amount of seconds that should pass
*		    		between each invocation of @action_func
*   @return value:  Returns the unique uid of the newly added task.
*   @error: 		In the event that this function failed to add 
			    	a new task it will return @bad_uid 
		    		that is defined externally.
*			    	Undefined behavior if @scheduler is not valid or
*			    	@action_func is not valid
*   @time complex: 	O(n) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
ilrd_uid_t SchedulerAdd(scheduler_t* scheduler, 
							int (*action_func)(void* params), void* params,
							size_t interval_in_sec);


/* 
*   @desc:          Removes a task from @scheduler identified by @identifier
*				    Cannot be used in a task to remove itself from @scheduler.
                    Use the action func return value to remove a running task from
*				    @scheduler 
*   @params: 		@scheduler: pre allocated scheduler
*				    @identifier: identifier to search for task to remove
*   @return value:  zero if found and removed the task and
*				    nonzero if failed to find the task
*   @error: 		Undefined behavior if @scheduler is invalid
*   @time complex: 	O(n) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
int SchedulerRemove(scheduler_t* scheduler, ilrd_uid_t identifier);


/* 
*   @desc:          Starts running @scheduler or if already running will return
*				    @RUNNING status code
*   @params: 		@scheduler: pre allocated scheduler
*   @return value:  status code of the running process:
*				    @SUCCESS means it ended successfully without getting called
*				    to stop
*				    @STOPPED means a function called for the scheduler to stop
*				    @ERROR means an error occurred while running the scheduler
*				    @DESTROYED means a function called for destroy during the
*				    running of the scheduler
*   @error: 		Undefined behavior if @scheduler is not valid
*   @time complex: 	O(n * m) for both AC/WC
*   @space complex: O(m) for both AC/WC
*/
status_t SchedulerRun(scheduler_t* scheduler);


/* 
*   @desc:          Sends a signal to the scheduler to stop @scheduler
*   @params: 		@scheduler: pre allocated scheduler
*   @return value:  None
*   @error: 		Undefined behavior if @scheduler is invalid
*   @time complex: 	O(1) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
void SchedulerStop(scheduler_t* scheduler);

/* 
*   @desc:          Counts the amount of tasks currently in @scheduler
*   @params: 		@scheduler: pre allocated scheduler
*   @return value:  Returns the count of tasks in @scheduler
*   @error: 		Undefined behavior if @scheduler is invalid
*   @time complex: 	O(n) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
size_t SchedulerSize(const scheduler_t* scheduler);

/* 
*   @desc:          Checks whether @scheduler is empty
*   @params: 		@scheduler: pre allocated scheduler
*   @return value:  Returns 1 if the scheduler doesn't have tasks and
*					0 otherwise
*   @error: 		Undefined behavior if @scheduler is invalid
*   @time complex: 	O(1) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
int SchedulerIsEmpty(const scheduler_t* scheduler);

/* 
*   @desc:          Removes all tasks from @scheduler. If it is called from
*					a task it will remove all the tasks besides the caller
*   @params: 		@scheduler: pre allocated scheduler
*   @return value:  None
*   @error: 		Undefined behavior if scheduler is invalid
*   @time complex: 	O(n) for both AC/WC
*   @space complex: O(1) for both AC/WC
*/
void SchedulerClear(scheduler_t* scheduler);

#endif /* __SCHEDULER_HEAP_H__ */       

