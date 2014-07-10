#include "grid_init.h"


void set_random_grid(Grid* grid){

    // **DESCRIPTION**: Set full random grid of three states: CONIFEROUS,
    // DECIDUOUS, MIXED.

    for (y, y < grid->yDim, y++) {
        for (x, x < grid->xDim, x++) {

            // Assign location cell value in currentCell
            GridCell * currentCell = grid_get_cell(x,y);
            int rValue = gsl_rng_uniform_int(rng, unsigned long int 3);

            // Set state based on the random value
            if(rValue == 0){
                        currentCell->currentState = currentCell->stateHistory = CONIFEROUS;
            } else if (rValue == 1) {
                        currentCell->currentState = currentCell->stateHistory = DECIDUOUS;
            } else if (rValue == 2) {
                        currentCell->currentState = currentCell->stateHistory = MIXED;
            }

            set_disturb_grid();
            gsl_rng_free(rng);
        }
    }
}

void set_uniform_grid(Grid* grid){

    // **DESCRIPTION**: This function create a landscape with each third (on Y
    // axis) of the grid is filled by one state : D, M or C

    // Get y grid dimension
    int ysize = grid->yDim

    for (x, x < grid->xDim, x++) {
            for (y, y < grid->yDim, y++) {

                GridCell * currentCell = grid_get_cell(x,y);

                if(y < (ysize/3)){
                    currentCell->currentState = currentCell->stateHistory = DECIDUOUS;
                }
                else if ( y < 2*(ysize/3) ){
                    currentCell->currentState = currentCell->stateHistory = MIXED;
                }
                else if ( y < ysize ){
                    currentCell->currentState = currentCell->stateHistory = CONIFEROUS;
                }
                else {
                    abort(); // Helpful ?
                }

        }
    }

    set_disturb_grid()

}

void set_disturb_grid( Grid* grid, double thresDist){

    double thresDist = 0.20; // Set

    int totalCells = grid->yDim * grid->yDim; // Get total number of cells
    int ysize = grid->yDim
    int xsize = grid->xDim
    int numDist = totalCells * thresDist; // Get number of cells disturbed based on threshold

    for (i, i < numDist, i ++){

        int ryCoord = gsl_rng_uniform_int(rng, unsigned long int ysize);
        int rxCoord = gsl_rng_uniform_int(rng, unsigned long int xsize);

        GridCell * currentCell = grid_get_cell(rxCoord,ryCoord);
        currentCell->currentState = currentCell->stateHistory = TRANSITIONAL;
    }

    gsl_rng_free(rng);

}
