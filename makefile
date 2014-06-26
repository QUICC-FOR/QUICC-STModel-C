CC = clang


# builds and runs all tests
test: test/grid_cell_test
	test/grid_cell_test


#
#	source files
#

grid_cell.o: grid_cell.c grid_cell.h
	$(CC) -c -o grid_cell.o grid_cell.c
	

#
#	test suite
#

test/grid_cell_test: test/grid_cell_test.c grid_cell.o
	$(CC) -c -o grid_cell_test.o grid_cell_test.c
	$(CC) -o grid_cell_test grid_cell_test.o grid_cell.o
	
