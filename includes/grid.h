/*
	API definition for the grid of a cellular automaton model
*/

#ifndef GRID_H
#define GRID_H

#include "grid_cell.h"

#include <assert.h>	// for assert()
#include <stdlib.h> // for size_t

/*
	DATA STRUCTURES
*/

// this is where you write a comment explaining what random vs mixed vs uniform mean

typedef enum {RANDOM, UNIFORM, MIX,GRID_NULL} GridType;
typedef enum { MOORE, VONNE} NeighType;

// refactor to use stdbool.h
// need to change uses of TRUE to true and FALSE to false
typedef int bool;
#define TRUE 1
#define FALSE 0

typedef struct {
	size_t xDim;
	size_t yDim;
	GridCell ** gridData;
} Grid;


/*
	FUNCTION PROTOTYPES
*/

// TODO: add parameters and function description

// refactor:
// move "private" functions to .c and make static
Grid * gr_make_grid(size_t xsize, size_t ysize, size_t numTimeSteps, GridType gridType, bool disturbances,gsl_rng* rng);
GridCell * gr_get_cell(Grid * grid, size_t x, size_t y);
void gr_set_cell(Grid* grid, State chosenState, size_t x, size_t y);
void gr_get_neighbor_states(Grid *grid, State *dest, size_t x, size_t y, NeighType neighType);
void gr_compute_prevalence(Grid * grid, size_t x, size_t y, NeighType neighType);
void gr_destroy_grid(Grid * grid);
void gr_set_disturb_grid(Grid *grid, double thresDist, gsl_rng *rng);
void gr_set_null_grid(Grid *grid, gsl_rng *rng);
void gr_output(Grid *grid);

#endif
