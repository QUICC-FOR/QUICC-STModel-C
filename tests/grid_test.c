#include <math.h>
#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid.c"
#include "../src/grid_cell.c"

void gr_test_alloc();
void gr_test_init();
void gr_test_get();
void gr_test_neighbors();

int main() {
	
	gr_test_alloc();
	gr_test_init();
	gr_test_get();
	gr_test_neighbors();

	return EXIT_SUCCESS;

}


void gr_test_alloc() {

	/* DESC: Test memory allocated to the grid */

	// init random number
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 

	size_t xSize = 10;
	size_t ySize = 8;
	size_t timeSteps = 100;

	// Create grid
	Grid * grid = gr_make_grid(xSize,ySize,timeSteps,UNIFORM,rng);
	assert(grid);


	assert(grid->xDim == xSize);
	assert(grid->yDim == ySize);
	assert(grid->gridData != NULL);
	
	gr_destroy_grid(grid);

}

void gr_test_init() {
}

void gr_test_get() {
}

void gr_test_neighbors () {
}