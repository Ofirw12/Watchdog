
#include <fcntl.h> /*O_CREAT, O_RDWR*/
#include <pthread.h> /*pthread_t*/
#include <semaphore.h> /* semaphore*/
#include <signal.h> /*kill, sigaction*/
#include <stdio.h> /*fprintf*/
#include <stdlib.h> /*setenv*/
#include <unistd.h> /*fork, exec*/
#include <sys/stat.h> /*unsetenv*/
#include <sys/types.h> /*pid_t*/

#include "watchdog.h"
#include "inner_watchdog.h"
#include "scheduler_heap.h" /* scheduler */

#define BUFFSIZE (64)

static pthread_t thread = {0};
static pid_t pid = {0};
static sem_t* sem = NULL;
static size_t thread_params[2];
static char** user_args = NULL;
static int user_argc = 0;

/*static functions declarations*/

static void* ThreadFunc(void* arg);
static char** SetForkArgs(char* threshold_buffer, char* interval_buffer
                                                ,int argc, char** argv);

/*static functions definitions*/

static void* ThreadFunc(void* arg)
{
    (void)arg;
    CreateWD(thread_params[0], thread_params[1],
                                        CLIENT, user_argc, user_args);
    return NULL;
}

static char** SetForkArgs(char* threshold_buffer, char* interval_buffer
                                                ,int argc, char** argv)
{
    size_t i = 0;
    char** final_argv = (char**) malloc(sizeof(char*) * (argc + SERVER_ARGS + 1));

    if (!final_argv)
    {
        return NULL;
    }

    final_argv[0] = argv[1];
    final_argv[1] = threshold_buffer;
    final_argv[2] = interval_buffer;
    
    for (i = 0; i < (size_t)argc; i++)
    {
        final_argv[SERVER_ARGS + i] = argv[i];
    }
    
    final_argv[SERVER_ARGS + i] = NULL;

    return final_argv;
}

/*functions definitions*/

wd_status_t StartWD(size_t threshold, size_t interval, int argc, char** argv)
{
    char pid_buffer[BUFFSIZE];
    char threshold_buffer[BUFFSIZE];
    char interval_buffer[BUFFSIZE];
    char** fork_args = NULL;
    pthread_attr_t attr = {0};

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    thread_params[0] = threshold;
    thread_params[1] = interval;

    sprintf(threshold_buffer, "%lu", threshold);
    sprintf(interval_buffer, "%lu", interval);

    user_args = argv;
    user_argc = argc;
    fork_args = SetForkArgs(threshold_buffer, interval_buffer, argc, argv);

    sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 0);

    pid = fork();

    if (-1 == pid)
    {
        sem_close(sem);
        free(fork_args);
        return WD_FAILURE; /*process creation failed*/
    }

    if (0 == pid)
    {
        if (-1 == execvp(fork_args[0],fork_args))
        {
            sem_close(sem);
            free(fork_args);
            return WD_FAILURE; /*error in exec*/
        }
    }
    sem_wait(sem);
    sprintf(pid_buffer, "%d", pid);
    if (0 != setenv(ENV_VAR, pid_buffer, 1))
    {
        sem_close(sem);
        free(fork_args);
        return WD_FAILURE; /*env issue*/
    }

    if (0 != pthread_create(&thread, &attr, ThreadFunc, NULL))
    {
        /*return error*/
        kill(pid, SIGUSR2);
        sem_close(sem);
        free(fork_args);
        return WD_FAILURE; /*thread creation failed*/
    }
    sem_wait(sem);
    free(fork_args);
    sem_close(sem);

    return WD_SUCCESS;
}

void StopWD(void)
{
    pid = (pid_t)atoi(getenv(ENV_VAR));
    kill(pid, SIGUSR2);
    raise(SIGUSR2);
    pthread_join(thread, NULL);
    
    unsetenv(ENV_VAR);
    sem_unlink(SEMAPHORE_NAME);
}