cmake_minimum_required(VERSION 3.15.2)
add_definitions("-Wall -std=c++11")

project(ipn VERSION 0.0.1 DESCRIPTION "inter-process notification")
add_library(ipn SHARED
    src/client.cpp
    src/server.cpp
)

set_target_properties(ipn PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ipn PROPERTIES SOVERSION 1)
set_target_properties(ipn PROPERTIES
    PUBLIC_HEADER
    include/ipn.hpp
    include/client.hpp
    include/message.hpp
)

find_package(Boost 1.70.0 REQUIRED system)
find_package(msgpack REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}" "${Boost_INCLUDE_DIRS}")
target_include_directories(ipn PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(ipn system ${msgpack_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS ipn
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Add sub-directories
add_subdirectory(example/server)
add_subdirectory(example/client)