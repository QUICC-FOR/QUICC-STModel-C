#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"
#include "../src/grid.c"


int main() {

	/* DESC:  Tests on prevalence function */

	// set up RNG
	  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	  assert(rng);
	  gsl_rng_set(rng, (int)time(NULL));

	  // number of iterations
	int nTest = 10;
	bool disturb = FALSE;

	//  Start iterations, test memory allocation for 10 grids with size and timeSteps randomly setup; 

	for(int z = 0; z < nTest; z++){
		// vars declaration	
		size_t xSize = gsl_rng_uniform_int(rng, 99)+1;
		size_t ySize = gsl_rng_uniform_int(rng, 99)+1;
		size_t timeSteps = gsl_rng_uniform_int(rng, 99)+1;


		// Init grid
		Grid *grid = gr_make_grid(xSize,ySize,timeSteps, UNIFORM,disturb,rng);
		gr_view_grid(grid);
		for(int x = 0 ; x < xSize; x++){
			for(int y = 0 ; y < ySize; y++){
				gr_compute_prevalence(grid, x, y,  MOORE);

				double sum_prev = 0.0;

				for(int i =0; i < GC_NUM_STATES; i++){	
					sum_prev += gr_get_cell(grid,x,y)->prevalence[i];
				}

				assert(sum_prev == 1.0);
		}
		
		}
			// Free memory
			gr_destroy_grid(grid);
	}

		gsl_rng_free(rng);
}
