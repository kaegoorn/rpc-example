cmake_minimum_required(VERSION 3.5)

project(problem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(problem main.cpp
    abstractcommand.h
    abstracttransport.h
    commandfactory.h
    commandprocessor.h
    protocol.h
    types.h
)

target_link_libraries(problem PRIVATE Threads::Threads)
