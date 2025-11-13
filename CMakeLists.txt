cmake_minimum_required(VERSION 3.31)
project(LinkedList)

set(CMAKE_CXX_STANDARD 20)

SET( CMAKE_CXX_FLAGS "-O2 -DNDEBUG -mtune=native -march=native")

add_executable(LinkedList main.cpp
        LinkedList.h
        NonLockingLinkedList.h
        benchmark.cc
)
