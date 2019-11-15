cmake_minimum_required(VERSION 3.15.2)

project(subscriber)
set(CMAKE_CXX_STANDARD 11)

add_executable(subscriber main.cpp)

target_link_libraries(subscriber ipn)
target_link_libraries(subscriber cppzmq)

target_include_directories(subscriber INTERFACE ../../include)