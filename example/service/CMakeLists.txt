cmake_minimum_required(VERSION 3.15.2)

project(service)
set(CMAKE_CXX_STANDARD 11)

add_executable(service main.cpp)

target_link_libraries(service ipn)
target_link_libraries(service cppzmq)

target_include_directories(service INTERFACE ../../include)