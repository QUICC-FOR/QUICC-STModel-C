#include "grid.h"
#include <gsl/gsl_rng.h>

// **FUNCTIONS PROTOTYPES**

void set_random_grid(Grid* grid){

    // **DESCRIPTION**: Set full random grid of three states: CONIFEROUS,
    // DECIDUOUS, MIXED. Remove state: TRANSITIONNAL

    for (y, y < grid->yDim, y++) {
        for (x, x < grid->xDim, x++) {

            // Assign location cell value in currentCell
            GridCell * currentCell = grid_get_cell(x,y);

            //Alloc memory for a random number
            gsl_rng * rng = gsl_rng_alloc(gsl_rng_mt19937);

            // Set state based on the random value
            switch((int)(gsl_rng_set(rng, (int) * 10.0) )) // Number between 0 and 100
                {
                    case 0:        // this is 0.0 <= rng_value < 0.25
                            currentCell->currentState = currentCell->stateHistory = TRANSITIONAL;
                        break;
                    case 25:        // this is 0.25 <= rng_value < 0.50
                            currentCell->currentState = currentCell->stateHistory = CONIFEROUS;
                        break;
                    case 50:        // this is 0.50 <= rng_value < 0.75
                            currentCell->currentState = currentCell->stateHistory = DECIDUOUS;
                        break;
                    case 75:        // this is 0.75 <= rng_value < 1
                            currentCell->currentState = currentCell->stateHistory = MIXED;
                        break;
                    default:
                        abort();
                        break;
                }

            gsl_rng_free(rng)
        }
    }
}

void set_uniform_grid(Grid* grid){

    // **DESCRIPTION**: This function create a landscape with each third (on Y
    // axis) of the grid is filled by one of states : D, M or C

    // Get y grid dimension
    int yGridDim = grid->yDim

    for (x, x < grid->xDim, x++) {
            for (y, y < grid->yDim, y++) {

                GridCell * currentCell = grid_get_cell(x,y);

                if(y < (yGridDim/3)){
                    currentCell->currentState = currentCell->stateHistory = DECIDUOUS;
                }
                else if ( y < 2*(yGridDim/3) ){
                    currentCell->currentState = currentCell->stateHistory = MIXED;
                }
                else if ( y < yGridDim ){
                    currentCell->currentState = currentCell->stateHistory = CONIFEROUS;
                }
                else {
                    abort(); // Helpfull ?
                }

        }
    }
}

//void set_mixed_grid( Grid* grid){



//}

//void set_disturb_grid( Grid* grid){

    // DESCRIPTION:  The idea behind this function aims to create a grid with
    // randomly a proportion of patch disturbed in the landscape

//}


GridCell * grid_get_cell(Grid * grid, size_t x, size_t y) {

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

Grid * grid_make_grid(size_t xsize, size_t ysize, GridType gridType) {

    // **PARAMETERS**

    // xsize and ysize: Grid dimensions
    // gridType : Initial conditions of the grid
    // OPTIONS:
        // 1. Randomly
        // 2. each state (except T) corresponds to one third distributed along the grid

    int dim = xsize * ysize;
    Grid * newGrid = malloc(sizeof Grid);

    assert(newGrid);

    newGrid->xDim = xsize;
    newGrid->yDim = ysize;

    // Alloc memory

    newGrid->gridData = malloc(dim * sizeof GridCell)
    assert(newGrid->gridData);
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

