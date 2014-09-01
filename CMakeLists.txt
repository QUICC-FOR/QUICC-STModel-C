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

################################################################
# TESTS ########################################################

set(all_tests "")

# Macro for adding tests:

macro(add_librandamu_test NAME)
	list(APPEND all_tests ${NAME})
	add_executable(${NAME} tests/${NAME}.c)
	target_link_libraries(${NAME} m)
	add_test(NAME ${NAME} COMMAND ${NAME})
endmacro(add_librandamu_test)

add_librandamu_test(test_gr_make_grid)
