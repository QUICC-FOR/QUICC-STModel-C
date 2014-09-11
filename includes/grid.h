/*
	API definition for the grid of a cellular automaton model
*/

#ifndef GRID_H
#define GRID_H

#include "grid_cell.h"

#include <assert.h>	// for assert()
#include <stdlib.h> // for size_t
#include <stdbool.h>

/*
	DATA STRUCTURES
*/

// this is where you write a comment explaining what random vs mixed vs uniform mean

typedef enum {RANDOM, UNIFORM, MIX, GRID_NULL} StartingConditionType;
typedef enum {MOORE, VONNE} NeighborhoodType;

typedef struct {
	size_t xDim;
	size_t yDim;
	NeighborhoodType neighborhood;
	GridCell ** gridData;
} Grid;


/*
	FUNCTION PROTOTYPES
*/

// TODO: add parameters and function description

void gr_update_all_cells(Grid * grid, gsl_rng *rng);
Grid * gr_make_grid(size_t xsize, size_t ysize, size_t numTimeSteps, NeighborhoodType nbType,
		StartingConditionType startingCondition, double startDisturbanceRate, gsl_rng* rng);
void gr_destroy_grid(Grid * grid);
void gr_output(Grid *grid);
void gr_view_grid(Grid *grid);

/* The functions below provide an iterator-like interface to the grid
	This can be used to loop through the grid without worrying about x-y coordinates */
GridCell * gr_first(Grid * grid);
GridCell * gr_next(Grid * grid, GridCell * cell);

#endif
