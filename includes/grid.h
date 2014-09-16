#ifndef GRID_H
#define GRID_H

#include "climate.h"

/* 
	define some constants controlling error conditions when grids are created
	GR_MAX_DIM_SIZE determines the maximum size of each dimension of the grid
	GR_MAX_DISTURBANCE_RATE defines the maximum allowable disturbance rate
*/
#define GR_MAX_DIM_SIZE 65535
#define GR_MAX_DISTURBANCE_RATE 0.60
#define GR_NUM_STATES 4
extern const char GR_POSSIBLE_STATES [GR_NUM_STATES];
//typedef enum {RANDOM, UNIFORM, MIX, GRID_NULL} GrStartingConditionType;
typedef enum {RANDOM, UNIFORM, MIX, GR_NULL} GrStartingConditionType;
typedef enum {MOORE, VONNE} GrNeighborhoodType;

//#include "grid_cell.h"

//#include <stdlib.h> // for size_t

/*
	DATA STRUCTURES
*/

/*
	The StateData type is intended to hold any type of data where there should be one
	value for each type (i.e., the prevalence of types in the neighborhood)
*/
typedef double StateData [GR_NUM_STATES];


typedef struct {
	short x;
	short y;
} OffsetType;


typedef struct {
	unsigned short xdim, ydim;
	unsigned short nbsize;
	OffsetType * nbOffsets;
	
	/*
		The grid state is stored in stateCurrent and stateNext
		These are two-dimensional char arrays [x][y] indexing
		spatially, the program assumes x maps to -1*longitude and y maps to latitude
		therefore, small y refers to the south, large y refers to the north
		small x is EAST, large x is west
		
		at the beginning of a timestep, stateCurrent is the current state of the grid
		and stateNext is garbage
		following updating all cells; stateNext will contain the next time step
		the head pointers can then be swapped with the gr_advance_state function to
		"update" the current state with the next state
	*/
	char ** stateCurrent;
	char ** stateNext;
} Grid;




/*
	FUNCTION PROTOTYPES
*/

// TODO: add parameters and function description

void gr_update_cell(Grid * grid, int x, int y, Climate * currClimate, ClimatePars * climPars, gsl_rng *rng);
Grid * grid_from_file(unsigned int xsize, unsigned int ysize, GrNeighborhoodType nbType, const char * gridDataFile);
Grid * gr_make_grid(unsigned int xsize, unsigned int ysize, GrNeighborhoodType nbType,
	GrStartingConditionType startingCondition, double startDisturbanceRate, gsl_rng *rng);
void gr_destroy_grid(Grid * grid);
void gr_output(Grid *grid);
void gr_view_grid(Grid *grid);
void gr_advance_state(Grid * gr);

#endif
