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
	size_t xSize = 10;
	size_t ySize = 10;
	size_t timeSteps = 10;


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

	// init array to store neighbor states of the cell
	State *neighborStates = malloc(nbSize * sizeof(State));

  	gr_get_neighbor_states(grid, neighborStates, xSize/2, ySize/2, MOORE);



  	// Compare states returned to value content in cells 



	// Free memory
	gr_destroy_grid(grid);
	
}