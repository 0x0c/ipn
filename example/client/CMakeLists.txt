cmake_minimum_required(VERSION 3.15.2)

project(client)
set(CMAKE_CXX_STANDARD 11)

add_executable(client main.cpp)

target_link_libraries(client ipn)
target_link_libraries(client cppzmq)

target_include_directories(client INTERFACE ../../include)