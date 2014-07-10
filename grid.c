#include "grid.h"
#include "grid_cell.h"
#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

/* FUNCTION PROTOTYPES */

void gr_set_random_grid(Grid* grid);
//void gr_set_mixed_grid(Grid* grid);
void gr_set_uniform_grid(Grid* grid);
void gr_set_disturb_grid( Grid* grid, double thresDist);


GridCell * gr_get_cell(Grid * grid, size_t x, size_t y) {

	// always protect your pointers, boys and girls
	assert(grid);

	// basic error checking, making sure we don't exceed our max dimensions
	// we use less than because arrays are 0 indexed, so y = yDim is an error
	assert(y < grid->yDim);
	assert(x < grid->xDim);

	// note the arrow operator: grid->yDim === (*grid).yDim
	size_t index = grid->yDim * y + x;

	// for a 10x10 grid (100 element array), giving it values of 9,9 results in 9*10 + 9 = 99, which is the last element
	// (5,3) results in an index of 3 * 10 + 5 = 35

	// finally, note that this function makes no guarantees that the object returned is not null
	// it's not technically required to check this, since this function is not dereferencing the pointer that is returned
	// however, this means the calling code will need to check the result; a one-line way to do this would be:
	//GridCell * myCell;0.

	//assert(myCell = grid_get(myGrid, xx, yy));
	// the assertion will fail if myCell gets a null pointer; assuming it succeeds, myCell can be safely used
	return grid->gridData[index];
}


GridCell * gr_set_cell (Grid * grid, GridCell * value, size_t x, size_t y){

	// pointers validation
	assert(grid);
	assert(value);

}


GirdCell ** gr_get_neighbors(Grid * grid, x, y){ // Output: two dimensional array ?

	int neighborsArr[8]; // Moore neighbors without original cell x,y

	// pointers validation
	assert(grid);

	// Store neighbors pointer addresses or direct value ?

}

Grid * gr_make_grid(size_t xsize, size_t ysize, GridType gridType) {

	int dim = xsize * ysize;
	Grid * newGrid = malloc(sizeof Grid);

	assert(newGrid);

	newGrid->xDim = xsize;
	newGrid->yDim = ysize;

	// **Alloc memory**

	newGrid->gridData = malloc(dim * sizeof GridCell)
	assert(newGrid->gridData);

	// **Alloc memory GridCell level**

	// for loop across all gridData and call


	// newGrid->gridData[i] = make_cell()

	switch( gridType ) {
		case RANDOM:
			set_random_grid(newGrid);
			break;

		case MIXED:
			set_mixed_grid(newGrid);
			break;

		case UNIFORM:
			set_uniform(newGrid);
			break;

		default:
			abort()
			break;
		 }

	// Generate grid

	return newGrid

}



/*	 	GRID INITIALIZATION FUNCTIONS		*/



void gr_set_random_grid(Grid* grid){

	State chosenState;

	for (y, y < grid->yDim, y++) {
		for (x, x < grid->xDim, x++) {
			// Pickup a random state
			chosenState = gsl_ran_choose(rng, &chosenState, 1, GC_POSSIBLE_STATES, GC_NUM_STATES, sizeof(State));
			// Set state based on the random value
			gr_set_cell(x,y) = chosenState;
		}
	}

gsl_rng_free(rng);

}

void gr_set_uniform_grid(Grid* grid){

	// Get y dimension
	int ysize = grid->yDim

	for (x, x < grid->xDim, x++) {
		for (y, y < grid->yDim, y++) {

			if(y < (ysize/3)){
				gr_set_cell(x,y) = DECIDUOUS;
			}
			else if ( y < 2*(ysize/3) ){
				gr_set_cell(x,y) =  MIXED;
			}
			else if ( y < ysize ){
				gr_set_cell(x,y) =  CONIFEROUS;
			}
			else {
				abort(); // Helpful ?
			}

		}
	}

	set_disturb_grid();

}

void gr_set_disturb_grid( Grid* grid, double thresDist){

	double thresDist = 0.20; // Set as Global variable ?

	int totalCells = grid->yDim * grid->yDim; // Get total number of cells
	int numDist = totalCells * thresDist; // Get number of cells disturbed based on threshold

	// Get grid dimensions
	int ysize = grid->yDim
	int xsize = grid->xDim

	for (i, i < numDist, i ++){

		int ryCoord = gsl_rng_uniform_int(rng, unsigned long int ysize);
		int rxCoord = gsl_rng_uniform_int(rng, unsigned long int xsize);
		gr_set_cell(rxCoord,ryCoord) = TRANSITIONAL;
	}

	gsl_rng_free(rng);

}


