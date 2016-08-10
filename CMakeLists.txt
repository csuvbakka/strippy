cmake_minimum_required (VERSION 2.8)
project (strippy)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -D_POSIX_C_SOURCE=200112L")

include_directories(include)
add_executable(proxy src/main.c)

add_library(socket src/strippy_socket.c)
target_link_libraries(proxy socket)
