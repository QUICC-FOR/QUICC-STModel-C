#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include "grid.h"
#include "grid_cell.h"

// sets a max disturbance rate; if set too high, will take a very long time to find cells
// to disturb
static const double GR_MAX_DISTURBANCE_RATE = 0.60; 

typedef struct {
	short int x;
	short int y;
} OffsetType;


/* FUNCTION PROTOTYPES */
static GridCell * gr_get_cell(Grid * grid, size_t x, size_t y);
static void gr_compute_prevalence(Grid *grid, GridCell * cell);
static OffsetType * gr_get_neighborhood_offsets(Grid * grid);
static void gr_set_random_grid(Grid *grid, gsl_rng *rng);
static void gr_set_null_grid(Grid *grid);
static void gr_set_uniform_grid(Grid *grid);
static void gr_set_mixed_grid(Grid *grid, gsl_rng *rng);
static void gr_set_disturb_grid(Grid *grid, double distRate, gsl_rng *rng);
static void ot_set(OffsetType * o, short x, short y);



GridCell * gr_first(Grid * grid)
{
	return gr_get_cell(grid, 0, 0);
}



GridCell * gr_next(Grid * grid, GridCell * cell)
{
	assert(grid && cell);
	size_t newx = cell->x + 1;
	size_t newy = cell->y;
	if(newx >= grid->xDim) {
		newx = 0;
		newy++;
	}

	if(newy >= grid->yDim)
		return NULL;		
	return gr_get_cell(grid, newx, newy);
}



void gr_update_all_cells(Grid * grid, gsl_rng *rng)
{
	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {
		gr_compute_prevalence(grid, currentCell);
		gc_get_trans_prob(currentCell);
		gc_select_next_state(currentCell, rng);
    }
    
	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell))
		gc_update_current_state(currentCell);
}




static GridCell *gr_get_cell(Grid *grid, size_t x, size_t y) {
	assert(grid);
	assert(y <= grid->yDim);
	assert(x <= grid->xDim);

	size_t index = grid->yDim * x + y;
	return grid->gridData[index];
}




static void gr_compute_prevalence(Grid *grid, GridCell * cell) 
{
	assert(grid && cell);

	size_t nbSize = (grid->neighborhood == MOORE ? 8 : 4);
	double increment = 1.0 / nbSize;
	OffsetType * offsets = gr_get_neighborhood_offsets(grid);

	// reset prevalence to 0
	for(int i = 0; i < GC_NUM_STATES; i++) cell->prevalence[i] = 0.0;

	// Compute prevalence
	for(int i = 0; i < nbSize; i++) {
		int newx = cell->x + offsets[i].x;
		int newy = cell->y + offsets[i].y;
		State neighborState;
		// set up torus; if we go out of bounds in the y direction we assume the type is
		// deciduous if we are in the south and coniferous if in the north
		if(newy < 0) {
			neighborState = DECIDUOUS;
		} else if (newy >= grid->yDim) {
			neighborState = CONIFEROUS;
		} else {
			// for x, we just wrap around
			if(newx < 0) {
				newx = grid->xDim - 1;
			} else if(newx >= grid->xDim) {
				newx = 0;
			}
			neighborState = *(gr_get_cell(grid, newx, newy)->currentState);
		}
		cell->prevalence[neighborState] += increment;
	}
	free(offsets);
}




static OffsetType * gr_get_neighborhood_offsets(Grid * grid) 
{
	size_t nbSize = (grid->neighborhood == MOORE ? 8 : 4);
	OffsetType * offsets = malloc(nbSize * sizeof(*offsets));

	ot_set(&(offsets[0]),-1,0);
	ot_set(&(offsets[1]),1,0);
	ot_set(&(offsets[2]),0,-1);
	ot_set(&(offsets[3]),0,1);

	if(grid->neighborhood == MOORE) {
		ot_set(&(offsets[4]),-1,-1);
		ot_set(&(offsets[5]),-1,1);
		ot_set(&(offsets[6]),1,-1);
		ot_set(&(offsets[7]),1,1);
	}
	return offsets;
}




Grid * gr_make_grid(size_t xsize, size_t ysize, size_t numTimeSteps, NeighborhoodType nbType,
                   StartingConditionType startingCondition, double startDisturbanceRate, gsl_rng *rng) {

  assert(startDisturbanceRate > 0 && startDisturbanceRate <= GR_MAX_DISTURBANCE_RATE);
  Grid *newGrid = malloc(sizeof(*newGrid));
  assert(newGrid);
  newGrid->xDim = xsize;
  newGrid->yDim = ysize;
  newGrid->neighborhood = nbType;
  
  newGrid->gridData = malloc(xsize*ysize * sizeof(*(newGrid->gridData)));
  assert(newGrid->gridData);
  int i = 0;
  for (int x = 0; x < newGrid->xDim; x++) {
    for (int y = 0; y < newGrid->yDim; y++) {
      newGrid->gridData[i++] = gc_make_cell(numTimeSteps, x, y);
    }
  }

  // Setup initial spatial config of the grid
  switch (startingCondition) {
  case RANDOM:
    gr_set_random_grid(newGrid, rng);
    break;

  case UNIFORM:
    gr_set_uniform_grid(newGrid);
    break;

  case MIX:
    gr_set_mixed_grid(newGrid, rng);
    break;

  case GRID_NULL:
    gr_set_null_grid(newGrid);
    break;
  }

  gr_set_disturb_grid(newGrid, startDisturbanceRate, rng);

  return newGrid;
}




void gr_destroy_grid(Grid *grid) {
	for(int i = 0; i < (grid->xDim * grid->yDim); i++)
		gc_destroy_cell(grid->gridData[i]);
	free(grid->gridData);
	free(grid);
}




/*    GRID INITIALIZATION FUNCTIONS   */
static void gr_set_random_grid(Grid *grid, gsl_rng *rng) {
  State allowedStates [] = {CONIFEROUS, DECIDUOUS, MIXED};
	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {
      State * cellState = currentCell->currentState;
      gsl_ran_choose(rng, cellState, 1, allowedStates, 3, sizeof(State));
    }
}




// what is the purpose of this?
static void gr_set_null_grid(Grid *grid) {
	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell))
      *(currentCell->currentState) = 0;
}




static void gr_set_uniform_grid(Grid *grid) {
  assert(grid->xDim >= 3);
  assert(grid->yDim >= 3);

  // set latitudinal thresholds for each state
  int deciduousUpperLimit = grid->yDim / 3; // from 0 to 1/3
  int coniferousLowerLimit = (grid->yDim * 2) / 3; // from 2/3 to max

	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {
      if(currentCell->y < deciduousUpperLimit) {
        *(currentCell->currentState) = DECIDUOUS;
      } else if(currentCell->y >= coniferousLowerLimit) {
        *(currentCell->currentState) = CONIFEROUS;      
      } else {
        *(currentCell->currentState) = MIXED;      
      }
    }
}




static void gr_set_mixed_grid(Grid *grid, gsl_rng *rng) {
  assert(grid->xDim >= 5);
  assert(grid->yDim >= 5);
  State states [] = {MIXED, MIXED};

  int yIncrement = grid->yDim / 5;

	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {
      State * cellState = currentCell->currentState;
      if (currentCell->y < yIncrement) {
        *cellState = CONIFEROUS;      
      } else if (currentCell->y < 2 * yIncrement) {
        states[0] = CONIFEROUS;
        gsl_ran_choose(rng, cellState, 1, states, 2, sizeof(State));      
      } else if (currentCell->y < 3 * yIncrement) {
        *cellState = MIXED;      
      } else if (currentCell->y < 4 * yIncrement) {
        states[0] = DECIDUOUS;
        gsl_ran_choose(rng, cellState, 1, states, 2, sizeof(State));      
      } else {
        *cellState = DECIDUOUS;      
      }
    }
}




static void gr_set_disturb_grid(Grid *grid, double disturbanceRate, gsl_rng *rng) {
  if(disturbanceRate == 0)
  	return;

  size_t numToDisturb = grid->xDim * grid->yDim * disturbanceRate;
  size_t numberDisturbed = 0;
  while(numberDisturbed < numToDisturb) {
    int rxCoord = gsl_rng_uniform_int(rng, grid->xDim);
    int ryCoord = gsl_rng_uniform_int(rng, grid->yDim);
    State * curCellState = gr_get_cell(grid, rxCoord, ryCoord)->currentState;
    if(*curCellState != TRANSITIONAL) {
    	*curCellState = TRANSITIONAL;
    	numberDisturbed++;
    }
  }
}




void gr_output(Grid *grid){
	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {
        printf("%d,%d",(int)currentCell->x,(int)currentCell->y);
        for(int t =0; t<currentCell->historySize;t++){
          printf(",%c", st_state_to_char(currentCell->stateHistory[t]));
        }
	printf("\n");      
	}
}




void gr_view_grid(Grid *grid) {
	for(GridCell * currentCell = gr_first(grid); currentCell; currentCell = gr_next(grid, currentCell)) {
		fprintf(stderr,"| %c ", st_state_to_char(*(currentCell->currentState)));
		if(currentCell->x == (grid->xDim - 1))
		    fprintf(stderr,"|   %d\n", (int) currentCell->y);
	}
}



static void ot_set(OffsetType * o, short x, short y)
{
	o->x = x;
	o->y = y;
}
