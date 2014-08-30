cmake_minimum_required(VERSION 2.8)
project(stmodel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

include_directories(includes)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -std=c99")

add_executable(stmodel
  src/main.c
  src/grid.c
  src/grid_cell.c
)

target_link_libraries(stmodel m gsl gslcblas)

