cmake_minimum_required(VERSION 3.20)

project(dvector)
add_library(dvector_static STATIC dvector.c)
add_library(dvector_shared SHARED dvector.c)

project(heap)
add_library(heap_static STATIC heap.c)
add_library(heap_shared SHARED heap.c)
target_link_libraries(heap_static dvector_static)
target_link_libraries(heap_shared dvector_shared)

project(heap_pq)
add_library(heap_pq_static STATIC p_queue_heap.c)
add_library(heap_pq_shared SHARED p_queue_heap.c)
target_link_libraries(heap_pq_static heap_static)
target_link_libraries(heap_pq_shared heap_shared)

project(ilrd_uid)
add_library(ilrd_uid_static STATIC ilrd_uid.c)
add_library(ilrd_uid_shared SHARED ilrd_uid.c)

project(heap_scheduler)
add_library(heap_scheduler_static STATIC scheduler_heap.c)
add_library(heap_scheduler_shared SHARED scheduler_heap.c)
target_link_libraries(heap_scheduler_static heap_pq_static task_static)
target_link_libraries(heap_scheduler_shared heap_pq_static task_shared)

project(task)
add_library(task_static STATIC task.c)
add_library(task_shared SHARED task.c)
target_link_libraries(task_static ilrd_uid_static)
target_link_libraries(task_shared ilrd_uid_shared)

project(watch_dog)
add_library(watch_dog_utils SHARED inner_watchdog.c watchdog.c)
target_link_libraries(watch_dog_utils heap_scheduler_shared)
add_executable(watch_dog.out inner_watchdog_main.c)
target_link_libraries(watch_dog.out watch_dog_utils heap_scheduler_shared)
