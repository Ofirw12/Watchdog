cmake_minimum_required(VERSION 3.20)

project(heap_scheduler)
add_executable(heap_scheduler test_scheduler_heap.c)
target_link_libraries(heap_scheduler heap_scheduler_static)

project(watch_dog)
add_executable(user_app test_watchdog.c)
target_link_libraries(user_app watch_dog_utils heap_scheduler_shared)
