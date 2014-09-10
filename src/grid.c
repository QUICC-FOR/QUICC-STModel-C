#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include "grid.h"
#include "grid_cell.h"

static const double THRESHDIST = 0.20;

/* FUNCTION PROTOTYPES */
// refactor: make static
void gr_set_random_grid(Grid *grid, gsl_rng *rng);
void gr_set_mixed_grid(Grid *grid, gsl_rng *rng);
void gr_set_uniform_grid(Grid *grid, gsl_rng *rng);
void gr_set_disturb_grid(Grid *grid, double thresDist, gsl_rng *rng);

static GridCell * gr_get_cell(Grid * grid, size_t x, size_t y);
static void gr_compute_prevalence(Grid *grid, GridCell * cell, size_t x, size_t y);




void gr_update_all_cells(Grid * grid, gsl_rng *rng)
{
	for (int x = 0; x < grid->xDim; x++) {
		for (int y = 0; y < grid->yDim; y++) {
			GridCell * currentCell = gr_get_cell(grid, x, y);
			gr_compute_prevalence(grid, currentCell, x, y);
			gc_get_trans_prob(currentCell);
			gc_select_new_state(currentCell, rng);
      }
    }
}




static GridCell *gr_get_cell(Grid *grid, size_t x, size_t y) {
	assert(grid);
	assert(y <= grid->yDim);
	assert(x <= grid->xDim);

	size_t index = grid->xDim * y + x;
	return grid->gridData[index];
}




static void gr_compute_prevalence(Grid *grid, GridCell * cell, size_t x, size_t y) 
{
  assert(grid);

  //  Fix number of neighbor cells
  size_t nbSize = (grid->neighborhood == MOORE ? 8 : 4);

  StateData prevalence;
  for(int i = 0; i < GC_NUM_STATES; i++) prevalence[i] = 0.0;
  double increment = 1.0 / nbSize;

  // set aside some memory for the neighbors
  State *neighborStates = malloc(nbSize * sizeof(State));
  assert(neighborStates);

  gr_get_neighbor_states(grid, neighborStates, x, y);

  // Compute prevalence
  for (int i = 0; i < nbSize; i++)
	prevalence[neighborStates[i]] += increment;

  for(int i = 0; i < GC_NUM_STATES; i++) cell->prevalence[i] = prevalence[i];

  free(neighborStates);
}




void gr_get_neighbor_states(Grid *grid, State *dest, size_t x, size_t y) 
{

  assert(y < grid->yDim);
  assert(x < grid->xDim);

  int i = 0;
  size_t xSize = grid->xDim - 1;
  size_t ySize = grid->yDim - 1;

  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {

      // EXCEPTIONS

      // if Cell located in the middle (0,0), skip it
      if (dx == 0 && dy == 0) {
        continue;
      }

      if (grid->neighborhood == VONNE) {
        // Skip cells unused in VON NEUMAN neighboors
        if (dx == -1 && dy == -1) {
          continue;
        } else if (dx == 1 && dy == 1) {
          continue;
        } else if (dx == 1 && dy == -1) {
          continue;
        } else if (dx == -1 && dy == 1) {
          continue;
        }
      }

      // new coord

      int new_x = x + dx;
      int new_y = y + dy;

      // static state boundaries on y
      if (new_y < 0) {
        dest[i] = DECIDUOUS;
      } else if (new_y > ySize) {
        dest[i] = CONIFEROUS;
      } else {
        // Torus on x
        if (new_x > xSize) {
          dest[i] = *(gr_get_cell(grid, 0, new_y)->currentState);
        } else if (new_x < 0) {
          dest[i] = *(gr_get_cell(grid, xSize - 1, new_y)->currentState);
        } else {
          dest[i] = *(gr_get_cell(grid, new_x, new_y)->currentState);
        }
      }

      i++; // Moove pos in dest array
    }
  }
}

// refactor: be sure fix malloc calls: obj = malloc(len * sizeof(*obj))


Grid * gr_make_grid(size_t xsize, size_t ysize, size_t numTimeSteps, NeighborhoodType nbType,
                   StartingConditionType startingCondition, bool startWithDisturbance, gsl_rng *rng) {

  int dim = xsize * ysize;
  Grid *newGrid = malloc(sizeof(*newGrid));
  assert(newGrid);
  newGrid->xDim = xsize;
  newGrid->yDim = ysize;
  newGrid->neighborhood = nbType;
  
  newGrid->gridData = malloc(dim * sizeof(*(newGrid->gridData)));
  assert(newGrid->gridData);
  for (int i = 0; i < dim; i++)
    newGrid->gridData[i] = gc_make_cell(numTimeSteps);

  // Setup initial spatial config of the grid
  switch (startingCondition) {
  case RANDOM:
    gr_set_random_grid(newGrid, rng);
    break;

  case UNIFORM:
    gr_set_uniform_grid(newGrid, rng);
    break;

  case MIX:
    gr_set_mixed_grid(newGrid, rng);
    break;

  case GRID_NULL:
    gr_set_null_grid(newGrid, rng);
    break;
  }

  if(startWithDisturbance)
    gr_set_disturb_grid(newGrid, THRESHDIST, rng);

  return newGrid;
}




void gr_destroy_grid(Grid *grid) {

  const size_t xsize = grid->xDim;
  const size_t ysize = grid->yDim;

  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
    	// refactor: no need to do this by x and y; can simply loop through the data directly
      GridCell *cell = gr_get_cell(grid, x, y);
      gc_destroy_cell(cell);
    }
  }

  free(grid->gridData);
  free(grid);
}

/*    GRID INITIALIZATION FUNCTIONS   */

void gr_set_random_grid(Grid *grid, gsl_rng *rng) {

  for (int x = 0; x < grid->xDim; x++) {
    for (int y = 0; y < grid->yDim; y++) {
      // Pickup a random state
      int rn = gsl_rng_uniform_int(rng, 3);
      // Set state based on the random value
      switch (rn) {
      case CONIFEROUS:
        gr_set_cell(grid, CONIFEROUS, x, y);
        break;

      case DECIDUOUS:
        gr_set_cell(grid, DECIDUOUS, x, y);
        break;

      case MIXED:
        gr_set_cell(grid, MIXED, x, y);
        break;
      }
    }
  }
}

void gr_set_null_grid(Grid *grid, gsl_rng *rng) {

  for (int x = 0; x < grid->xDim; x++) {
    for (int y = 0; y < grid->yDim; y++) {
      gr_set_cell(grid, 0, x, y);
    }
  }
}

void gr_set_uniform_grid(Grid *grid, gsl_rng *rng) {

  assert(grid->xDim >= 3);
  assert(grid->yDim >= 3);

  // Get y dimension
  int ylim = grid->yDim / 3;

  for (int x = 0; x < grid->xDim; x++) {
    for (int y = 0; y < grid->yDim; y++) {

      if (y < ylim) {
        gr_set_cell(grid, CONIFEROUS, x, y);
      } else if (y < 2 * ylim) {
        gr_set_cell(grid, MIXED, x, y);
      } else if (y >= 2 * ylim) {
        gr_set_cell(grid, DECIDUOUS, x, y);
      } else {
        printf("%s \n", "State undefined...");
        abort();
      }
    }
  }
}

// refactor this
void gr_set_mixed_grid(Grid *grid, gsl_rng *rng) {
  assert(grid->xDim >= 5);
  assert(grid->yDim >= 5);

  int ylim = grid->yDim / 5;

  for (int x = 0; x < grid->xDim; x++) {
    for (int y = 0; y < grid->yDim; y++) {

      int rn = gsl_rng_uniform_int(rng, 2);

      if (y < ylim) {
        gr_set_cell(grid, CONIFEROUS, x, y);
      } else if (y < 2 * ylim) {
        switch (rn) {

        case 0:
          gr_set_cell(grid, CONIFEROUS, x, y);
          break;

        case 1:
          gr_set_cell(grid, MIXED, x, y);
          break;
        }
      } else if (y < 3 * ylim) {
        gr_set_cell(grid, MIXED, x, y);
      } else if (y < 4 * ylim) {
        switch (rn) {

        case 0:
          gr_set_cell(grid, DECIDUOUS, x, y);
          break;

        case 1:
          gr_set_cell(grid, MIXED, x, y);
          break;
        }
      } else if (y >= 4 * ylim) {
        gr_set_cell(grid, DECIDUOUS, x, y);
      } else {
        printf("%s \n", "State undefined...");
        abort();
      }
    }
  }
}

// refactor: remove extra definitions; even better
// change THRESHDIST to a startingDisturbanceRate parameter that is passed to the constructor
// and defined in main
void gr_set_disturb_grid(Grid *grid, double thresDist, gsl_rng *rng) {

  thresDist = THRESHDIST;

  int totalCells = grid->xDim * grid->yDim; // Get total number of cells
  int numDist = totalCells *
                thresDist; // Get number of cells disturbed based on threshold

  for (int i = 0; i < numDist; i++) {
    int rxCoord = gsl_rng_uniform_int(rng, grid->xDim);
    int ryCoord = gsl_rng_uniform_int(rng, grid->yDim);
    gr_set_cell(grid, TRANSITIONAL, rxCoord, ryCoord);
  }
}

void gr_output(Grid *grid){

	// refactor: these variables can be eliminated
  size_t xsize = grid->xDim;
  size_t ysize = grid->yDim;

  for(int x =0 ; x < xsize;  x++){
    for(int y =0 ; y < ysize;  y++){
        GridCell *cell  = gr_get_cell(grid,x,y);
        printf("%d,%d",x,y);
        for(int t =0; t<cell->historySize;t++){
          printf(",%c", st_state_to_char(cell->stateHistory[t]));
        }
         printf("\n");    
       }    
  }
}

void gr_view_grid(Grid *grid) {
  for (int y = 0; y < grid->yDim; ++y) {
    for (int x = 0; x < grid->xDim; ++x) {
      GridCell *cell = gr_get_cell(grid, x, y);
      fprintf(stderr,"| %c ", st_state_to_char(*(cell->currentState)));
    }
    fprintf(stderr,"|   %d\n", y);
  }
}

// refactor this out of existence
void gr_set_cell(Grid *grid, State chosenState, size_t x, size_t y) {
  // pointers validation
  assert(grid);
  GridCell *cell = gr_get_cell(grid, x, y);
  *(cell->currentState) = chosenState;
}





