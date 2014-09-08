#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"
#include "../src/grid.c"

int main() {

	/* DESC: Test memory allocated to the grid */

	// Init random number generator
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 

	// number of iterations
	int nTest = 100;
	bool disturb = TRUE;

	//  Start iterations, test memory allocation for 10 grids with size and timeSteps randomly setup; 

	for(int i = 0; i < nTest+1; i++){
		// Random var		
		size_t xSize = gsl_rng_uniform_int(rng, 99)+1; // Last expression +1: Ensured grid min size != 0;
		size_t ySize = gsl_rng_uniform_int(rng, 99)+1; // Last expression +1: Ensured grid min size != 0;
		size_t timeSteps = 10;

		// Create grid
		Grid * grid = gr_make_grid(xSize,ySize,timeSteps,RANDOM,disturb,rng);
		assert(grid);

		// Assertions
		assert(grid->xDim == xSize);
		assert(grid->yDim == ySize);
		assert(grid->gridData != NULL);
		
		// Free memory
		gr_destroy_grid(grid);
		 gsl_rng_free(rng);
	}
}
