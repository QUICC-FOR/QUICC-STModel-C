// #define NDEBUG   // enable this to switch off debugging/assertions

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include <gsl/gsl_rng.h>

#include "grid.h"

// set global modelling constants
static const int MAX_TIME = 2000;
static const int GR_SIZE = 5;
static const NeighborhoodType NBTYPE = MOORE;
static const double DISTURB_RATE = 0.20;

int main(int argc, char **argv) {

  // set up RNG
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
  assert(rng);
  gsl_rng_set(rng, (int)time(NULL));

  // set up the grid
  Grid *grid = gr_make_grid(GR_SIZE, GR_SIZE+2, MAX_TIME, NBTYPE, UNIFORM, DISTURB_RATE, rng);

  // main loop in time
  for (int year = 0; year < MAX_TIME; year++) {
  	gr_update_all_cells(grid, rng);
  }
  gr_output(grid);

  // cleanup
  gr_destroy_grid(grid);
  gsl_rng_free(rng);
}
