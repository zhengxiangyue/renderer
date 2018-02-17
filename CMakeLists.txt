cmake_minimum_required(VERSION 3.9)
project(graphic C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework GLUT -framework OpenGL") # compiler version and compiler flags.

add_subdirectory(lib)
INCLUDE_DIRECTORIES(lib)

add_library(homework1 homework1)
add_library(homework2 homework2)
add_executable(graphic main)

target_link_libraries(homework1 myGraphics)
target_link_libraries(homework2 myGraphics)
target_link_libraries(graphic homework1)
target_link_libraries(graphic homework2)