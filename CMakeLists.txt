cmake_minimum_required(VERSION 3.15.2)
add_definitions("-Wall")
set(CMAKE_CXX_STANDARD 11)

project(ipn VERSION 0.0.1 DESCRIPTION "inter-process notification")
add_library(ipn INTERFACE)

include_directories("${CMAKE_SOURCE_DIR}/include")

find_package(cppzmq REQUIRED)
find_package(msgpack REQUIRED)

target_include_directories(ipn INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(ipn ${msgpack_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS ipn
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Add sub-directories
add_subdirectory(example/publisher)
add_subdirectory(example/subscriber)
add_subdirectory(example/service)
add_subdirectory(example/client)