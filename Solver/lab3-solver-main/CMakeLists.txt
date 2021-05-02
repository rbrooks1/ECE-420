cmake_minimum_required(VERSION 3.10)
project(lab3_solver C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fopenmp")

add_executable(main Code/src/main.c Code/lib/Lab3IO.c)