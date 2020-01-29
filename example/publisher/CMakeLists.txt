cmake_minimum_required(VERSION 3.15.2)

project(publisher)
set(CMAKE_CXX_STANDARD 11)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../ReqRep.proto)
add_executable(publisher main.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(publisher ipn)
target_link_libraries(publisher cppzmq)
target_link_libraries(publisher ${Protobuf_LIBRARIES})

target_include_directories(publisher INTERFACE ../../include)