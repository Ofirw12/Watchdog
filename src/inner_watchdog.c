
#include <fcntl.h> /*O_CREAT, O_RDWR*/
#include <semaphore.h> /* semaphore*/
#include <signal.h> /*kill, sigaction*/
#include <stdio.h> /*FILE, sprintf*/
#include <stdlib.h> /*atoi, getenv, setenv*/
#include <unistd.h> /*getppid*/
#include <sys/types.h>


#include "inner_watchdog.h"

#define SERVER_PROC ("debug/wd.out")
#define CLIENT_PROC ("debug/user.out")
#define LOGGER_PATH ("debug/wd_logger.txt")
#define BUFSIZE (64)

#define NAME(num) ((CLIENT == num) ? ("Client") : ("Server"))

static sem_t* sem = NULL;
static watchdog_t watchdog = {0};

#ifndef NDEBUG
    char buffer[BUFSIZ];
#endif

/*static functions declarations*/
static void ReviveServer(void);
static void ReviveClient(void);
static void SigUSR1Handler(int sig);
static void SigUSR2Handler(int sig);
static int PulseTest(void* params);
static char** SetForkArgs(char* threshold_buffer, char* interval_buffer);

/*static functions*/
static void SigUSR1Handler(int sig)
{
    (void)sig;
    atomic_store(&watchdog.counter, 0);

    {
        atomic_store(&watchdog.is_running, 0);
    }
    #ifndef NDEBUG
        sprintf(buffer, "%s: got signal SIGUSR1 from pid %d\n",NAME(watchdog.role), watchdog.send_to);
        LogAction(buffer);
    #endif
}

static void SigUSR2Handler(int sig)
{
    (void)sig;
    atomic_store(&watchdog.is_running, 0);

    #ifndef NDEBUG
        sprintf(buffer, "%s: got signal SIGUSR2 from pid %d\n",NAME(watchdog.role), watchdog.send_to);
        LogAction(buffer);
    #endif
}

static int RegularSignalSender (void* params)
{
    (void)params;
    atomic_fetch_add(&watchdog.counter, 1);
    #ifndef NDEBUG
        sprintf(buffer, "%s: sent a signal SIGUSR1 to %s, pid:%d\n", NAME(watchdog.role), NAME(!watchdog.role), watchdog.send_to);
        LogAction(buffer);
    #endif
    kill(watchdog.send_to, SIGUSR1);

    return 0;
}

static int PulseTest(void* params)
{
    watchdog_t watchdog = *(watchdog_t*)params;
    if (watchdog.threshold < atomic_load(&watchdog.counter))
    {
        SchedulerStop(watchdog.sched);
    }

    if (!atomic_load(&watchdog.is_running))
    {
        DestroyWD();
    }

    return 0;
}

static char** SetForkArgs(char* threshold_buffer, char* interval_buffer)
{
    size_t i = 0;
    char** final_argv = (char**) malloc(sizeof(char*) * (watchdog.argc + SERVER_ARGS + 1));

    if (!final_argv)
    {
        return NULL;
    }

    final_argv[0] = watchdog.args[1];
    final_argv[1] = threshold_buffer;
    final_argv[2] = interval_buffer;
    
    for (i = 0; i < (size_t)watchdog.argc; i++)
    {
        final_argv[SERVER_ARGS + i] = watchdog.args[i];
    }
    
    final_argv[SERVER_ARGS + i] = NULL;

    return final_argv;
}

static void ReviveServer(void)
{
    pid_t new_pid = {0};
    char interval_buffer[BUFSIZE];
    char threshold_buffer[BUFSIZE];
    char pid_buffer[BUFSIZE];
    char** temp = NULL;
    kill(watchdog.send_to, SIGUSR2);
    #ifndef NDEBUG
        sprintf(buffer, "%s: sent a signal SIGUSR2 to %s, pid:%d\n", NAME(watchdog.role), NAME(!watchdog.role), watchdog.send_to);
        LogAction(buffer);
    #endif

    do
    {
        new_pid = fork();
    } while (-1 == new_pid);

    sprintf(interval_buffer, "%lu", watchdog.interval);
    sprintf(threshold_buffer, "%lu", watchdog.threshold);
    temp = watchdog.args;
    watchdog.args = SetForkArgs(threshold_buffer, interval_buffer);

    if (0 == new_pid)
    {
        execvp(watchdog.args[0], watchdog.args);
    }
    sprintf(pid_buffer, "%d", new_pid);
    setenv(ENV_VAR, pid_buffer, 1);
    #ifndef NDEBUG
        sprintf(buffer, "Client: new server pid is %s\n", getenv(ENV_VAR));
        LogAction(buffer);
    #endif
    sem_wait(sem);
    atomic_store(&watchdog.counter, 0);
    watchdog.send_to = (pid_t)atoi(getenv(ENV_VAR));
    free(watchdog.args);
    watchdog.args = temp;
}

static void ReviveClient(void)
{
    DestroyWD();
    sem_unlink(SEMAPHORE_NAME);
    execvp(watchdog.args[SERVER_ARGS], watchdog.args + SERVER_ARGS);
}

/*header functions:*/

int CreateWD(size_t threshold, size_t interval, wd_role role, int argc, char** args)
{
    struct sigaction action = {0};

    action.sa_handler = SigUSR1Handler;
    if (-1 == sigaction(SIGUSR1, &action, NULL))
    {
        return -1;
    }
    action.sa_handler = SigUSR2Handler;

    if (-1 == sigaction(SIGUSR2, &action, NULL))
    {
        return -1;
    }

    sem = sem_open(SEMAPHORE_NAME, O_RDWR);
    if (!sem)
    {
        return -1;
    }

    atomic_init(&watchdog.counter, 0);
    watchdog.interval = interval;
    watchdog.role = role;
    watchdog.threshold = threshold;
    atomic_init(&watchdog.is_running, 0);
    watchdog.sched = SchedulerCreate();
    watchdog.args = args;
    watchdog.argc = argc;

    if (!watchdog.sched)
    {
        sem_close(sem);
        sem_unlink(SEMAPHORE_NAME);
        return -1;
    }

    switch (role)
    {
        case CLIENT:
            watchdog.send_to = atoi(getenv(ENV_VAR));
            break;
        
        default:
            watchdog.send_to = getppid();
            break;
    }

    SchedulerAdd(watchdog.sched, RegularSignalSender, NULL, watchdog.interval);
    SchedulerAdd(watchdog.sched, PulseTest, (void*)&watchdog, watchdog.interval);
    sem_post(sem);
    RunWD();
    return 0;
}

void RunWD(void)
{
    atomic_store(&watchdog.is_running, 1);
    #ifndef NDEBUG
        sprintf(buffer, "(%s) - %s: started running on pid %d\n", __TIME__, NAME(watchdog.role), getpid());
        LogAction(buffer);
    #endif
    if (STOPPED == SchedulerRun(watchdog.sched))
    {
        #ifndef NDEBUG
            sprintf(buffer, "(%s) - %s: my buddy dieded, reviving\n",__TIME__ , NAME(watchdog.role));
            LogAction(buffer);
        #endif
        switch (watchdog.role)
        {
        case CLIENT:
            ReviveServer();
            RunWD();
            break;
        
        default:
            ReviveClient();
            break;
        }
    }
}

void DestroyWD(void)
{
    #ifndef NDEBUG
        sprintf(buffer, "(%s) - %s: Recieved SIGUSR2 and finished running\n", __TIME__, NAME(watchdog.role));
        LogAction(buffer);
    #endif
    SchedulerDestroy(watchdog.sched);
    sem_close(sem);
}

void LogAction(char* data)
{
    FILE* fp = fopen(LOGGER_PATH, "a");

    if (!fp)
    {
        return;
    }
    if (EOF == fputs(data, fp))
    {
        return;
    }
    fclose(fp);
}
