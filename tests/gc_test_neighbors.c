#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"
#include "../src/grid.c"

int main() {

	/* DESC: Test gr_get_neighbor_states */

	// Init random number generator

	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 

	// Random var		
	size_t xSize = gsl_rng_uniform_int(rng, 100);
	size_t ySize = gsl_rng_uniform_int(rng, 100);
	size_t timeSteps = gsl_rng_uniform_int(rng, 100);


	  //  Fix number of neighbor cells
	  NeighType neighType = MOORE;
	  size_t nbSize;

	  if(neighType == MOORE){
	    nbSize = 8;  
	  } 
	  else if(neighType == VONNE){
	    nbSize = 4;  
	  }
	  

	// Create grid
	Grid * grid = gr_make_grid(xSize,ySize,timeSteps,UNIFORM,rng);
	assert(grid);
	gr_view_grid(grid);

	// Take coordinates of the middle cell
	size_t xMid= xSize/2,  yMid= ySize/2;

	// init and fill array with neighbor states of the cell
	State *neighborStates = malloc(nbSize * sizeof(State));
  	gr_get_neighbor_states(grid, neighborStates, xMid, yMid, MOORE);

  	// Compare state returned (by the function) to the state value content in the cell of the grid 

  	assert(*(gr_get_cell(grid, xMid, yMid - 1)->currentState)==neighborStates[0]);


	// Free memory
	gr_destroy_grid(grid);
	
}
