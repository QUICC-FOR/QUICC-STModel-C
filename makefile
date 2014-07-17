CC = clang
GSL_INCLUDE = -I /usr/local/include
GSL_LINK = -lgsl -lgslcblas -L /usr/local/lib

all: fourstatemodel test

# builds and runs all tests
test: test/test_suite
	test/test_suite

# main model binary
fourstatemodel: main.o grid_cell.o
	$(CC) -o fourstatemodel main.o grid_cell.o

#
#	source files
#

main.o: main.c grid.h grid_cell.h
	$(CC) -c -o main.o main.c

grid_cell.o: grid_cell.c grid_cell.h
	$(CC) -c -o grid_cell.o grid_cell.c

grid.o: grid.c grid.h grid_cell.h
	$(CC) -c -o grid.o grid.c

#
#	test suite
#

test/test_suite: test/test.o test/grid_cell_test.o grid_cell.o test/grid_test.o grid.o
	$(CC) $(GSL_LINK) -o test/test_suite test/test.o test/grid_cell_test.o grid_cell.o

test/test.o: test/test.c test/test.h
	$(CC) -c -o test/test.o test/test.c

test/grid_cell_test.o: test/grid_cell_test.c test/test.h grid_cell.h
	$(CC) -c -o test/grid_cell_test.o test/grid_cell_test.c

test/grid_test.o: test/grid_test.c test/test.h grid.h
	$(CC) -c -o test/grid_test.o test/grid_test.c