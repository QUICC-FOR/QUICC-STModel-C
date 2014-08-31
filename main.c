// #define NDEBUG   // enable this to switch off debugging/assertions

#include "grid.h"
// #include "io.h"
#include "grid_cell.h"

#include <gsl/gsl_rng.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

// length of time to run the model
#define MAX_TIME 100;
#define OUTPUT_FILE "test_output.csv"

int main(int argc, char ** argv){

    // set up RNG
    gsl_rng* rng = gsl_rng_alloc(gsl_rng_mt19937);
    assert(rng);
    gsl_rng_set(rng, (int) time(NULL));

    // set up the grid
        // Grid * grid = gr_make_grid(10,10,100,RANDOM,rng);
    // 
    //         // size_t ysize;
    //         // size_t xsize;
    // 
    //         // grid->xDim = xsize;
    //         // grid->yDim = ysize;
    //         // 
    //         // for(int x;x<xsize;x++){
    //         //     for(int y; y<ysize;y++){
    //         //         printf("%p", gr_get_cell(grid,x,y));
    //         //     }
        // }

    // main loop in time
    // for(int year = 0; year < MAX_TIME; year++) {
    //  gr_update_grid(grid);
    // }

    //io_write_output(grid, OUTPUT_FILE);

    // cleanup
    gr_destroy_grid(grid);
    gsl_rng_free(rng);

    return 0;
}