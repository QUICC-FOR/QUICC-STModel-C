// #define NDEBUG   // enable this to switch off debugging/assertions

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include <gsl/gsl_rng.h>

#include "grid.h"
#include "grid_cell.h"

// length of time to run the model

#define MAX_TIME 100;
#define OUTPUT_FILE "test_output.csv"

int main(int argc, char **argv) {

  // set up RNG
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
  assert(rng);
  gsl_rng_set(rng, (int)time(NULL));

  // set up the grid
  Grid *grid = gr_make_grid(10, 10, 100, UNIFORM, rng);
  gr_view_grid(grid);

  // main loop in time
  for (int year = 0; year < 100; year++) {
    gr_update_grid(grid);
  }

  // io_write_output(grid, OUTPUT_FILE);

  // cleanup
  // gr_destroy_grid(grid);
  // gsl_rng_free(rng);

  return 0;
}
