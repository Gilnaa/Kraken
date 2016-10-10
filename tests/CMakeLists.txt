cmake_minimum_required(VERSION 3.6)
project(Kraken)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(REPO ~/proj/.repo)

include(CTest)

enable_testing()

include(${REPO}/DownloadProject/DownloadProject.cmake)
download_project(
        PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        UPDATE_DISCONNECTED 1
)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

include_directories(. ../include)

file(GLOB_RECURSE TEST_SOURCE_FILES *.cpp *.h)

add_executable(kraken-tests ${TEST_SOURCE_FILES})
target_link_libraries(kraken-tests gtest_main gtest gmock kraken)