cmake_minimum_required(VERSION 3.15.2)

project(ipn_server)
set(CMAKE_CXX_STANDARD 11)

add_executable(ipn_server main.cpp)
target_link_libraries(ipn_server ipn)
target_include_directories(ipn_server PRIVATE ../../include)