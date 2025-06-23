
#include <assert.h>
#include <stdio.h> /*FILE*/
#include <unistd.h> /*sleep*/

#include "watchdog.h"

#define THRESHOLD (5)
#define INTERVAL (1)
#define ROUNDS (1000000)

int main(int argc, char* argv[])
{
    size_t i = 0;
    char* buffer = "Testing\n";

    assert(argc > 1);

    StartWD(THRESHOLD, INTERVAL, argc, argv);

    for ( i = 0; i < ROUNDS; i++)
    {
        fprintf(stdout, "%s%lu\n", buffer, i);
    }
   
    StopWD();
    return 0;
}