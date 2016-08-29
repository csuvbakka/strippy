cmake_minimum_required (VERSION 2.8)
project (strippy)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z -Wall -g --coverage")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++1z -Wall -O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

set(GTEST_ROOT "${PROJECT_SOURCE_DIR}/../googletest/googletest")
# set(TEST_LIB "libgtest.a")
# set(MOCK_LIB "libgmock.a")

link_directories("${PROJECT_SOURCE_DIR}/lib")
include_directories(include)
add_executable(proxy main.cpp)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")
add_library(stuff ${SOURCES} ${HEADERS})
target_link_libraries(proxy stuff)
target_link_libraries(proxy pthread)

file(GLOB TEST_SOURCES "test/*.cpp")
include_directories("${GTEST_ROOT}/include")
add_executable(strippy_test ${TEST_SOURCES})
target_link_libraries(strippy_test stuff)
target_link_libraries(strippy_test gtest)
target_link_libraries(strippy_test pthread)
target_link_libraries(strippy_test gtest_main)
