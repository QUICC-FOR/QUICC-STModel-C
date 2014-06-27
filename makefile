CC = clang
GSL_INCLUDE = -I /usr/local/include
GSL_LINK = -lgsl -lgslcblas -L /usr/local/lib

# builds and runs all tests
test: test/grid_cell_test
	test/grid_cell_test
	test/grid_test


#
#	source files
#

grid_cell.o: grid_cell.c grid_cell.h
	$(CC) -c -o grid_cell.o grid_cell.c

# -c : Library (not linked files)
# -o : produce output
#
#	test suite
#

test/grid_cell_test: test/grid_cell_test.c grid_cell.o
	$(CC) -c -o test/grid_cell_test.o test/grid_cell_test.c
	$(CC) -o test/grid_cell_test test/grid_cell_test.o grid_cell.o

