cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)

project(lab4_pagerank C)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_C_COMPILER mpicc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -lm")

add_executable(main Code/src/main.c Code/lib/Lab4_IO.c)

target_link_libraries(main m ${MPI_LIBRARIES})
