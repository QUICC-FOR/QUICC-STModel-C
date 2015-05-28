cmake_minimum_required(VERSION 2.8)
project(stmodel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

enable_testing()

include_directories(includes)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg -fno-inline -O3 -g -Wall -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -Wall -std=c99")

add_executable(stmodel
  src/main.c
  src/grid.c
  src/climate.c
)

target_link_libraries(stmodel m gsl gslcblas)

################################################################
# TESTS ########################################################

set(all_tests "")

macro(add_libstmodel_test NAME)
  list(APPEND all_tests ${NAME})
  add_executable(${NAME} tests/${NAME}.c)
  target_link_libraries(${NAME} m gsl gslcblas)
  add_test(NAME ${NAME} COMMAND ${NAME})
endmacro(add_libstmodel_test)

add_libstmodel_test(gr_test_alloc)
add_libstmodel_test(gr_test_prevalence)
add_libstmodel_test(gr_test_new_state)
add_libstmodel_test(gr_test_trans_prob)
add_libstmodel_test(cl_test)

set(CMAKE_CXX_FLAGS "-O0")
