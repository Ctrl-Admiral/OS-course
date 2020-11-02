cmake_minimum_required(VERSION 3.5)

project(os-shmem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libOS libOS.cpp libOS.hpp)

add_executable(client1 client1.cpp)
add_executable(client2 client2.cpp)
add_executable(server server.cpp)

target_link_libraries(client1 libOS)
target_link_libraries(client2 libOS)
target_link_libraries(server libOS)
