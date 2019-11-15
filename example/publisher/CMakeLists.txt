cmake_minimum_required(VERSION 3.15.2)

project(publisher)
set(CMAKE_CXX_STANDARD 11)

add_executable(publisher main.cpp)

find_package(cppzmq REQUIRED)
target_link_libraries(publisher cppzmq)

target_include_directories(publisher INTERFACE ../../include)