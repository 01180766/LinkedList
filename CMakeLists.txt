cmake_minimum_required(VERSION 3.31)
project(LinkedList)

set(CMAKE_CXX_STANDARD 20)

add_executable(LinkedList main.cpp
        LinkedList.h
        NonLockingLinkedList.c
        NonLockingLinkedList.h
)
