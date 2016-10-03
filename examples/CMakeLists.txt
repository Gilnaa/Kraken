cmake_minimum_required(VERSION 3.6)
project(Kraken)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../include)

add_executable(01_files.elf 01_files.cpp)
target_link_libraries(01_files.elf kraken)

add_executable(02_sockets.elf 02_sockets.cpp)
target_link_libraries(02_sockets.elf kraken)



