cmake_minimum_required(VERSION 3.15.0)

project(subscriber)
set(CMAKE_CXX_STANDARD 11)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../ReqRep.proto)
add_executable(subscriber main.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(subscriber ipn)
target_link_libraries(subscriber cppzmq)
target_link_libraries(subscriber ${Protobuf_LIBRARIES})

target_include_directories(subscriber INTERFACE ../../include)
