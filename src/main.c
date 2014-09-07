// #define NDEBUG   // enable this to switch off debugging/assertions

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include <gsl/gsl_rng.h>

#include "grid.h"
#include "grid_cell.h"

// length of time to run the model

#define  MAX_TIME   10
#define GR_SIZE 10
#define OUTPUT_FILE "test_output.csv"

int main(int argc, char **argv) {

  // set up RNG
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
  assert(rng);
  gsl_rng_set(rng, (int)time(NULL));

  // set up the grid
  Grid *grid = gr_make_grid(GR_SIZE, GR_SIZE, MAX_TIME, UNIFORM, rng);
  gr_view_grid(grid);

  // main loop in time
  for (int year = 0; year < MAX_TIME; year++) {
    
    // View grid
    printf(" T ------------------------------------------------ \n");
    gr_view_grid(grid);
    for(int x =0; x < GR_SIZE; x++){
        for(int y =0; y < GR_SIZE; y++){

            //Compute prevalence
            gr_compute_prevalence(grid, x, y,  NEIGHTYPE);

            //Compute trans probabilities
            gc_get_trans_prob (gr_get_cell(grid,x,y));
            gc_select_new_state (gr_get_cell(grid,x,y), rng);
        }
    }

    // View grid
    printf(" T+1 ------------------------------------------------ \n");
    gr_view_grid(grid);

  }
  // io_write_output(grid, OUTPUT_FILE);

  // cleanup
  gr_destroy_grid(grid);
  gsl_rng_free(rng);
}
