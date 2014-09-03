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

	// Init random number generator

	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 

	// number of iterations
	int nTest = 10;

	//  Start iterations, test memory allocation for 10 grids with size and timeSteps randomly setup; 

	for(int i = 0; i < nTest+1; i++){
		// Random var		
		size_t xSize = gsl_rng_uniform_int(rng, 100);
		size_t ySize = gsl_rng_uniform_int(rng, 100);
		size_t timeSteps = gsl_rng_uniform_int(rng, 100);

		// Create grid
		Grid * grid = gr_make_grid(xSize,ySize,timeSteps,UNIFORM,rng);
		assert(grid);

		// Assertions
		assert(grid->xDim == xSize);
		assert(grid->yDim == ySize);
		assert(grid->gridData != NULL);
		assert(gr_get_cell(grid,1,1)->historySize == timeSteps);
		
		// Free memory
		gr_destroy_grid(grid);
	}
}

void gr_test_init() {
	/* DESC:  Perform unit tests on initial lattice configuration (RANDOM, ) */

}

void gr_test_get() {
	/* DESC:  Unit test on gr_get_cell */
}

void gr_test_neighbors () {
	/* DESC:  Unit test on gr_get_neighbor_states and gr_compute_prevalence*/

}
