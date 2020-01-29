cmake_minimum_required(VERSION 3.15.2)

project(client)
set(CMAKE_CXX_STANDARD 11)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../ReqRep.proto)
add_executable(client main.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(client ipn)
target_link_libraries(client cppzmq)
target_link_libraries(client ${Protobuf_LIBRARIES})

target_include_directories(client INTERFACE ../../include)