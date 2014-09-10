// #define NDEBUG   // enable this to switch off debugging/assertions

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include <gsl/gsl_rng.h>

#include "grid.h"
// remove grid_cell.h with refactors below -- don't need this level of specificity in main
#include "grid_cell.h"

// set global modelling constants
static const int MAX_TIME = 2000;
static const int GR_SIZE = 50;
static const NeighType NEIGHTYPE = MOORE;
static const bool DISTURB = TRUE;

int main(int argc, char **argv) {

  // set up RNG
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
  assert(rng);
  gsl_rng_set(rng, (int)time(NULL));

  // set up the grid
  Grid *grid = gr_make_grid(GR_SIZE, GR_SIZE, MAX_TIME, UNIFORM, DISTURB, rng);

  // main loop in time
  for (int year = 0; year < MAX_TIME; year++) {

	// refactor this to be within grid.c
	// gr_update_all_cells(grid, NEIGHTYPE)
	// or possibly pass the NEIGHTYPE constant to the grid constructor (better?)
    for (int x = 0; x < GR_SIZE; x++) {
      for (int y = 0; y < GR_SIZE; y++) {

        // Compute prevalence of cell
        gr_compute_prevalence(grid, x, y, NEIGHTYPE);

        // Compute trans probabilities
        gc_get_trans_prob(gr_get_cell(grid, x, y));

        // Select the new state of the actual cell
        gc_select_new_state(gr_get_cell(grid, x, y), rng);
      }
    }
  }

  gr_output(grid);

  // cleanup
  gr_destroy_grid(grid);
  gsl_rng_free(rng);
}
