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
	size_t xSize = 20;
	size_t ySize = 20;
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
	Grid * grid = gr_make_grid(xSize,ySize,timeSteps,MIX,rng);
	assert(grid);
	gr_view_grid(grid);

	// Take coordinates of tested cells
	size_t xMid= xSize/2,  yMid= ySize/2;
	//size_t xUp= xSize,  yUp= ySize;
	//size_t xDown= 0,  yDown= ySize/2;	

	// init and fill array with neighbor states of the cell
	State *neighborStates = malloc(nbSize * sizeof(State));

	// Assertion of four cells in the corners and in the middle
  	gr_get_neighbor_states(grid, neighborStates, xMid, yMid, MOORE);
  	//gr_get_neighbor_states(grid, neighborStates, xMid, yDown, MOORE);
  	//gr_get_neighbor_states(grid, neighborStates, xDown, yMid, MOORE);
  	//gr_get_neighbor_states(grid, neighborStates, xDown, yUp, MOORE);
  	//gr_get_neighbor_states(grid, neighborStates, xUp, yDown, MOORE);
  	//gr_get_neighbor_states(grid, neighborStates, xUp, yUp, MOORE);
  	//gr_get_neighbor_states(grid, neighborStates, xDown, yDown, MOORE);
  	
	// Free memory
	gr_destroy_grid(grid);
	
}
