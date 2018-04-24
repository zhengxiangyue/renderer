cmake_minimum_required(VERSION 3.9)
project(graphic C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework GLUT -framework OpenGL") # compiler version and compiler flags.

add_subdirectory(lib)
INCLUDE_DIRECTORIES(lib)

add_library(engine engine.cpp)

# the only executable file is main.cpp
add_executable(graphic main)

target_link_libraries(engine myGraphics)
target_link_libraries(engine illumination)
target_link_libraries(graphic engine)
