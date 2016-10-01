cmake_minimum_required(VERSION 3.6)
project(Kraken)

add_subdirectory(tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h include/*.h)
add_library(kraken ${SOURCE_FILES})
target_compile_options(kraken PRIVATE -Werror -Wall -Wextra -Wsuggest-override -fno-exceptions -Wextra -nostdlib)

include_directories(src include .)


