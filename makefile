CC = clang
GSL_INCLUDE = -I /usr/local/include
GSL_LINK = -lgsl -lgslcblas -L /usr/local/lib

# builds and runs all tests
test: test/test
	test/test


#
#	source files
#

grid_cell.o: grid_cell.c grid_cell.h
	$(CC) -c -o grid_cell.o grid_cell.c
	

#
#	test suite
#

test/test: test/test.o test/grid_cell_test.o grid_cell.o 
	$(CC) $(GSL_LINK) -o test/test test/test.o test/grid_cell_test.o grid_cell.o 
	
test/test.o: test/test.c test/test.h
	$(CC) -c -o test/test.o test/test.c

test/grid_cell_test.o: test/grid_cell_test.c test/test.h grid_cell.h
	$(CC) -c -o test/grid_cell_test.o test/grid_cell_test.c

