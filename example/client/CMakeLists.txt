cmake_minimum_required(VERSION 3.15.2)

project(ipn_client)
set(CMAKE_CXX_STANDARD 11)

add_executable(ipn_client main.cpp)
target_link_libraries(ipn_client ipn)
target_include_directories(ipn_client PRIVATE ../../include)