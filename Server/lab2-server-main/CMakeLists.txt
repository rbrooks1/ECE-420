cmake_minimum_required(VERSION 3.10)
project(lab2-server C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -lpthread")

add_executable(main1 Code/src/main1.c)
add_executable(main2 Code/src/main2.c)
add_executable(main3 Code/src/main3.c Code/src/mylib_rwlock_t.c)
add_executable(main4 Code/src/main4.c Code/src/mylib_rwlock_t.c)

target_link_libraries(main1 pthread)
target_link_libraries(main2 pthread)
target_link_libraries(main3 pthread)
target_link_libraries(main4 pthread)