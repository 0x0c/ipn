cmake_minimum_required(VERSION 3.15.2)

project(monitoring)
set(CMAKE_CXX_STANDARD 11)

add_executable(monitoring main.cpp)

target_link_libraries(monitoring ipn)
target_link_libraries(monitoring cppzmq)

target_include_directories(monitoring INTERFACE ../../include)