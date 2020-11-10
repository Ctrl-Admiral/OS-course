cmake_minimum_required(VERSION 3.5)

project(Sockets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server server.cpp)
add_executable(client client.cpp)

add_library(libOS ../libOS/libOS.cpp ../libOS/libOS.hpp)


set(libOS_INCLUDE_DIR ../libOS)

target_include_directories(client PRIVATE ${libOS_INCLUDE_DIR})
target_include_directories(server PRIVATE ${libOS_INCLUDE_DIR})


target_link_libraries(client libOS)
target_link_libraries(server libOS)

