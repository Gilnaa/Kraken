cmake_minimum_required(VERSION 3.6)
project(Kraken)

add_subdirectory(tests)
add_subdirectory(examples)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h include/*.h)
add_library(kraken ${SOURCE_FILES} include/Kraken/Features.h)
target_compile_options(kraken PRIVATE -Werror -Wall -Wextra -Wsuggest-override -fno-exceptions -nostdlib)

include_directories(src include .)


