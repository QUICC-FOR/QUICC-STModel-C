#include "test.h"
#include "../grid.h"

static int gr_test_alloc();
static int gr_test_init();
static int gr_test_get();
static int gr_test_neighbors();

int gr_test_all() {
	int failedTests = 0;

	failedTests += gr_test_alloc();
	failedTests += gr_test_init();
	failedTests += gr_test_get();
	failedTests += gr_test_neighbors();

	test_func_print("  Grid Test Suite", failedTests);
	return failedTests;

}


static int gr_test_alloc() {
	int fail = 0;
	int xSize = 10;
	int ySize = 15;

	Grid * grid = gr_make_grid(xSize,ySize);
	assert(grid);
	
	fail += (grid->xDim == xSize);
	fail += (grid->yDim == ySize);
	fail += (grid->gridData != NULL);
	
	gr_destroy_grid(grid);
	
	test_func_print("    gr_test_alloc", fail);
	return fail;
}

static int gr_test_init() {
	int fail = 0;

	fail++;	//until this function is implemented
	test_func_print("    gr_test_init", fail);
	return fail;
}

static int gr_test_get() {
	int fail = 0;

	fail++;	//until this function is implemented
	test_func_print("    gr_test_get", fail);
	return fail;
}

static int gr_test_neighbors () {
	int fail = 0;

	fail++;	//until this function is implemented
	test_func_print("    gr_test_neighbors", fail);
	return fail;
}