#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include "../src/grid_cell.c"
#include "../src/grid.c"

int main() {
  /* DESC: Tests on prevalence function */
  // set up RNG
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
  assert(rng);
  gsl_rng_set(rng, (int)time(NULL));
  // number of iterations
  int nTest = 100;
  double disturb = 0.20;
  // Start iterations, test memory allocation for 10 grids with size and
  // timeSteps randomly setup;
  for (int i = 0; i < nTest; i++) {
    // vars declaration
    size_t xSize = gsl_rng_uniform_int(rng, 97) + 3;
    size_t ySize = gsl_rng_uniform_int(rng, 97) + 3;
    size_t timeSteps = gsl_rng_uniform_int(rng, 99) + 1;
    
    // Init grid
    Grid *grid = gr_make_grid(xSize, ySize, timeSteps, MOORE, RANDOM, disturb, rng);

	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {

        gr_compute_prevalence(grid, currentCell);
        double sum_prev = 0.0;
        
        for (int z = 0; z < GC_NUM_STATES; z++) {
          sum_prev += currentCell->prevalence[z];
        }
        assert(sum_prev == 1.0);
    }
    // Free memory
    gr_destroy_grid(grid);
  }
  gsl_rng_free(rng);
}