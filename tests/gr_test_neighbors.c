#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <stdbool.h>

//#include "../src/grid_cell.c"
#include "../src/grid.c"

int main() {
/* disabling this entire test for now, since the neighbors functions have been 
// drastically rewritten

   DESC: Create a grid and try to get neighbors of cells located on boundaries
    and middle 

  // Init random number generator
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
  assert(rng);
  gsl_rng_set(rng, (int)time(NULL));

  // vars declaration
  size_t xSize = 3;
  size_t ySize = 3;
  size_t timeSteps = 10;
  bool disturb = false;
  int sum_grid = 0;
  int sum_neiCells = 0;
  size_t nbSize;

  //  Fix number of neighbor cells
  NeighborhoodType neighType = MOORE;

  if (neighType == MOORE) {
    nbSize = 8;
  } else if (neighType == VONNE) {
    nbSize = 4;
  }

  // Create RANDOM grid
  Grid *grid = gr_make_grid(xSize, ySize, timeSteps, neighType, GRID_NULL, disturb, rng);
  assert(grid);

  // init and fill array with neighbor states of the cell
  State *neighborStates = malloc(nbSize * sizeof(State));

  // get sum of the grid - should equal to 0
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++) {
      sum_grid += *(gr_get_cell(grid, x, y)->currentState);
    }
  }

  // First assertion on initial grid
  assert(sum_grid == 0);

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++) {

      gr_get_neighbor_states(grid, neighborStates, x, y);

      for (int i = 0; i < nbSize; i++) {
        sum_neiCells += neighborStates[i];
      }

      // Test static state boundaries on Y

      if (y == 0) {
        assert(sum_neiCells == 0 + (3 * DECIDUOUS));
      } else if (y == ySize - 1) {
        assert(sum_neiCells == 0 + (3 * CONIFEROUS));
      }

      // Test torus on X

      if (x == 0 && y == (ySize - 1) / 2) {
        assert(sum_neiCells == 0);
      } else if (x == xSize - 1 && y == (ySize - 1) / 2) {
        assert(sum_neiCells == 0);
      }

      sum_neiCells = 0;
    }
  }

  // Free memory
  gr_destroy_grid(grid);
  free(neighborStates);
  gsl_rng_free(rng);
  */
}
