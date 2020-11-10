cmake_minimum_required(VERSION 3.5)

project(os_shmem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libOS ../libOS/libOS.cpp ../libOS/libOS.hpp)

add_executable(client1 client1.cpp)
add_executable(client2 client2.cpp)
add_executable(server server.cpp)

set(libOS_INCLUDE_DIR ../libOS)

target_include_directories(client1 PRIVATE ${libOS_INCLUDE_DIR})
target_include_directories(client2 PRIVATE ${libOS_INCLUDE_DIR})
target_include_directories(server PRIVATE ${libOS_INCLUDE_DIR})

target_link_libraries(client1 libOS)
target_link_libraries(client2 libOS)
target_link_libraries(server libOS)
