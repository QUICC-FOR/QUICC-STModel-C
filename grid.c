#include "grid.h"

// FUNCTIONS PROTOTYPES

void set_random_grid( Grid* grid){

    // do some cool stuff

    // for loop in  x,y
    GridCell * currentCell = grid_get_cell(x,y);
    currentCell->currentState = currentCell->stateHistory = TRANSITIONAL



}


void set_mixed_grid( Grid* grid){


}


void set_uniform_grid( Grid* grid){



}
/*
    an example of a getter function follows, assuming that the data are implemented as a 1-D array with row major ordering
    this is implementation, so it will eventually go in a .c file; included here as an example only
*/

// **GLOBAL VARIABLE**


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


    switch( gridType ) {
        case RANDOM:
            set_random_grid(newGrid);
            break;

        case MIXED
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

