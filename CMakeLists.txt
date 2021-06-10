cmake_minimum_required(VERSION 3.19)
project(UI_Set)

set(CMAKE_CXX_STANDARD 14)

add_executable(UI_Set main.cpp RC.h include/ILogger.h include/ISetControlBlock.h include/ISet.h include/IVector.h src/ILogger.cpp src/ISet.cpp src/IVector.cpp src/myLogger.cpp src/myLogger.h src/mySet.cpp src/mySet.h src/mySetControlBlock.cpp src/mySetControlBlock.h src/myVector.cpp src/myVector.h)