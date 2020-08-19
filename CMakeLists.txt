cmake_minimum_required(VERSION 3.10)
PROJECT (triplebuffer)

set(CMAKE_BUILD_TYPE Release)

find_package(Threads REQUIRED)

set(SOURCES main.cpp TripleBuffer.hpp)
add_executable(triplebuffer ${SOURCES})

target_link_libraries(triplebuffer PRIVATE Threads::Threads)