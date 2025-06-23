#ifndef __INNER_WATCHDOG_H__
#define __INNER_WATCHDOG_H__

#include <stdatomic.h> /*atomic_uint*/
#include <stddef.h>         /* size_t */

#include "scheduler_heap.h" /* scheduler */

#define SEMAPHORE_NAME ("/watchdog")
#define ENV_VAR ("WD_PID")
#define SERVER_ARGS (3)

typedef enum 
{
    CLIENT = 0,
    SERVER = 1
} wd_role;

typedef struct
{
    size_t threshold;
    size_t interval;
    wd_role role;
    scheduler_t* sched;
    atomic_uint counter;
    atomic_uint is_running;
    pid_t send_to;
    char** args;
    int argc;
} watchdog_t;

int CreateWD(size_t threshold, size_t interval, wd_role role, int argc, char** args);

void RunWD(void);

void DestroyWD(void);

void LogAction(char* data);

#endif  /*__INNER_WATCHDOG_H__*/