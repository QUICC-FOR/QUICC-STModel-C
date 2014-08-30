#include "grid.h"
#include "grid_cell.h"
#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#define THRESDIST 0.20

/* FUNCTION PROTOTYPES */

void gr_set_random_grid(Grid *grid, gsl_rng *rng);
void gr_set_mixed_grid(Grid *grid, gsl_rng *rng);
void gr_set_uniform_grid(Grid *grid, gsl_rng *rng);
void gr_set_disturb_grid(Grid *grid, double thresDist, gsl_rng *rng);

GridCell *gr_get_cell(Grid *grid, size_t x, size_t y) {

  assert(grid);

  assert(y < grid->yDim);
  assert(x < grid->xDim);

  // note the arrow operator: grid->yDim === (*grid).yDim
  size_t index = grid->yDim * y + x;

  return &(grid->gridData[index]);
}

void gr_set_cell(Grid *grid, State value, int x, int y) {
  // pointers validation
  assert(grid);
  GridCell *c = gr_get_cell(grid, x, y);
  *(c->currentState) = value;
}

void gr_compute_prevalence(Grid *grid, size_t x, size_t y) {

  // pointer validation
  assert(grid);

  size_t nbSize = 8;
  double count_D = 0.0, count_C = 0.0, count_T = 0.0, count_M = 0.0;
  double increment = 1 / nbSize;

  // set aside some memory for the neighbors
  State *neighborStates = malloc(nbSize * sizeof(State));

  gr_compute_neighbor_states(grid, neighborStates, x, y, nbSize);

  // Compute prevalence

  for (int i = 0; i < nbSize; i++) {
    switch (neighborStates[i]) {

    case TRANSITIONAL:
      count_T = count_T + increment;
      break;

    case MIXED:
      count_M = count_M + increment;
      break;

    case DECIDUOUS:
      count_D = count_D + increment;
      break;

    case CONIFEROUS:
      count_C = count_C + increment;
      break;

    default:
      break;
    }
  }

  // Stored in cell
  gr_get_cell(grid, x, y)->prevalence[0] = count_C;
  gr_get_cell(grid, x, y)->prevalence[1] = count_D;
  gr_get_cell(grid, x, y)->prevalence[2] = count_M;
  gr_get_cell(grid, x, y)->prevalence[3] = count_T;

  free(neighborStates);
}

void gr_compute_neighbor_states(Grid *grid, State *dest, size_t x, size_t y,
                                size_t neighborhoodSize) {

  dest[0] = *(gr_get_cell(grid, x, y - 1)->currentState);
  dest[1] = *(gr_get_cell(grid, x, y + 1)->currentState);
  dest[2] = *(gr_get_cell(grid, x + 1, y)->currentState);
  dest[3] = *(gr_get_cell(grid, x - 1, y)->currentState);

  if (neighborhoodSize > 4) {
    dest[4] = *(gr_get_cell(grid, x - 1, y + 1)->currentState);
    dest[5] = *(gr_get_cell(grid, x - 1, y - 1)->currentState);
    dest[6] = *(gr_get_cell(grid, x + 1, y + 1)->currentState);
    dest[7] = *(gr_get_cell(grid, x + 1, y - 1)->currentState);
  }
}

Grid *gr_make_grid(size_t xsize, size_t ysize, size_t numTimeSteps,
                   GridType gridType, gsl_rng *rng) {

  int dim = xsize * ysize;
  Grid *newGrid = malloc(sizeof(Grid));
  printf("1\n");
  fflush(stdout);
  assert(newGrid);

  newGrid->xDim = xsize;
  newGrid->yDim = ysize;

  // **Alloc memory**

  newGrid->gridData = malloc(dim * sizeof(GridCell));
  assert(newGrid->gridData);

  // **Alloc memory GridCell level**
  printf("2\n");
  fflush(stdout);

  // for loop across all gridData and call
  for (int i = 0; i < (xsize * ysize); i++) {
    newGrid->gridData[i] = *(gc_make_cell(numTimeSteps));
  }
  printf("3\n");
  fflush(stdout);

  switch (gridType) {
  case RANDOM:
    gr_set_random_grid(newGrid, rng);
    break;

  case UNIFORM:
    gr_set_uniform_grid(newGrid, rng);
    break;

  case MIX:
    gr_set_mixed_grid(newGrid, rng);
    break;

  default:
    abort();
    break;
  }
  printf("4\n");
  fflush(stdout);

  return newGrid;
}

void gr_destroy_cell(Grid *grid, size_t x, size_t y) {

  free(gr_get_cell(grid, x, y)); // gr_get_cell return a pointer on GridCell
}

void gr_destroy_grid(Grid *grid) {

  const size_t ysize = grid->xDim;
  const size_t xsize = grid->yDim;

  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      gr_destroy_cell(grid, x, y);
    }
  }

  free(grid);
}

/*	 	GRID INITIALIZATION FUNCTIONS		*/

void gr_set_random_grid(Grid *grid, gsl_rng *rng) {

  State chosenState;

  for (int x = 0; x < grid->xDim; x++) {
    for (int y = 0; y < grid->yDim; y++) {
      // Pickup a random state
      chosenState = gsl_ran_choose(rng, &chosenState, 1, GC_POSSIBLE_STATES,
                                   GC_NUM_STATES, sizeof(State));
      // Set state based on the random value
      gr_set_cell(grid, chosenState, x, y);
    }
  }
}

void gr_set_uniform_grid(Grid *grid, gsl_rng *rng) {

  // Get y dimension
  int ysize = grid->yDim;
  State chosenState;

  for (int x = 0; x < grid->xDim; x++) {
    for (int y = 0; y < grid->yDim; y++) {

      if (y < (ysize / 3)) {
        chosenState = DECIDUOUS;
        gr_set_cell(grid,chosenState,x,y);
      } else if (y < 2 * (ysize / 3)) {
        chosenState = MIXED;
        gr_set_cell(grid,chosenState,x,y);
      } else if (y < ysize) {
        chosenState = CONIFEROUS;
        gr_set_cell(grid,chosenState,x,y);
      } else {
        printf("%s \n", "State undefined...");
        abort();
      }
    }
  }

  gr_set_disturb_grid(grid, THRESDIST, rng);
}

void gr_set_mixed_grid(Grid *grid, gsl_rng *rng) {
}

void gr_set_disturb_grid(Grid *grid, double thresDist, gsl_rng *rng) {

  thresDist = THRESDIST;

  int totalCells = grid->yDim * grid->yDim; // Get total number of cells
  int numDist = totalCells *
                thresDist; // Get number of cells disturbed based on threshold

  for (int i = 0; i < numDist; i++) {
    int rxCoord = gsl_rng_uniform_int(rng, grid->xDim);
    int ryCoord = gsl_rng_uniform_int(rng, grid->yDim);
    gr_set_cell(grid, TRANSITIONAL, rxCoord, ryCoord);
  }
}

void gr_update_grid(Grid *grid) {
}
