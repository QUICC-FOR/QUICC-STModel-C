#include <math.h>
#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid.c"
#include "../src/grid_cell.c"

static int gr_test_alloc();
static int gr_test_init();
static int gr_test_get();
static int gr_test_neighbors();

int main() {
	
	int failedTests = 0;

	failedTests += gr_test_alloc();
	failedTests += gr_test_init();
	failedTests += gr_test_get();
	failedTests += gr_test_neighbors();

	return EXIT_SUCCESS;

}


static int gr_test_alloc() {

	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 

	int fail = 0;
	int xSize = 10;
	int ySize = 15;
	int timeSteps = 100;

	Grid *grid = gr_make_grid(xSize,ySize,timeSteps,UNIFORM,rng);
	assert(grid);
	
	fail += (grid->xDim == xSize);
	fail += (grid->yDim == ySize);
	fail += (grid->gridData != NULL);
	
	gr_destroy_grid(grid);

	return fail;
}

static int gr_test_init() {
	int fail = 0;

	fail++;	//until this function is implemented
	return fail;
}

static int gr_test_get() {
	int fail = 0;

	fail++;	//until this function is implemented
	return fail;
}

static int gr_test_neighbors () {
	int fail = 0;

	fail++;	//until this function is implemented
	return fail;
}