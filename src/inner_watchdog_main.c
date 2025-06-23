
#include <assert.h>
#include <stdlib.h> /*atoi*/

#include "inner_watchdog.h"
#include "scheduler_heap.h" /* scheduler */

int main(int argc, char* argv[])
{
    assert(argc > 1);
    CreateWD((size_t)atoi(argv[1]),
        (size_t)atoi(argv[2]), SERVER,
                                        argc, argv);
    
    return 0;
}