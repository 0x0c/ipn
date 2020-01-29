cmake_minimum_required(VERSION 3.15.2)

project(service)
set(CMAKE_CXX_STANDARD 11)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../ReqRep.proto)
add_executable(service main.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(service ipn)
target_link_libraries(service cppzmq)
target_link_libraries(service ${Protobuf_LIBRARIES})

target_include_directories(service INTERFACE ../../include)