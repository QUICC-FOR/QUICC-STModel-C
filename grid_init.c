#include "grid_init.h"


void set_random_grid(Grid* grid){

    // **DESCRIPTION**: Set full random grid of three states: CONIFEROUS,
    // DECIDUOUS, MIXED. Remove state: TRANSITIONAL

    for (y, y < grid->yDim, y++) {
        for (x, x < grid->xDim, x++) {

            // Assign location cell value in currentCell
            GridCell * currentCell = grid_get_cell(x,y);

            // Set state based on the random value

            // RETRIEVE for IF
            switch((int)(gsl_rng_set(rng, (int) * 10) )) // Number between 0 and 100
                {
                    case 25:        // this is 25 <= rng_value < 50
                            currentCell->currentState = currentCell->stateHistory = CONIFEROUS;
                        break;
                    case 50:        // this is 50 <= rng_value < 75
                            currentCell->currentState = currentCell->stateHistory = DECIDUOUS;
                        break;
                    case 75:        // this is 75 <= rng_value
                            currentCell->currentState = currentCell->stateHistory = MIXED;
                        break;
                    default:
                        abort();
                        break;
                }

            set_disturb_grid();
            gsl_rng_free(rng);
        }
    }
}

//void set_disturb_grid( Grid* grid){

    // DESCRIPTION:  The idea behind this function aims to create a grid with
    // randomly a proportion of patch disturbed in the landscape

//}

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

    int totalCell = grid->yDim * grid->yDim; // Get total number of cells
    int numDist = totalCell * thresDist; // Get number of cells disturbed based on threshold

    for (i, i < numDist, i ++){
        // get random x coord
        // get random y coord

        GridCell * currentCell = grid_get_cell(x,y);
        currentCell->currentState = currentCell->stateHistory = TRANSITIONAL;
    }
}
