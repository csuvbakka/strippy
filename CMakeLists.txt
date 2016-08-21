cmake_minimum_required (VERSION 2.8)
project (strippy)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++1z -Wall -O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

include_directories(include)
add_executable(proxy main.cpp)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")
add_library(socket ${SOURCES} ${HEADERS})
target_link_libraries(proxy socket)
target_link_libraries(proxy pthread)
