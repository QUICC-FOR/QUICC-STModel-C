/*
	API definition for the grid of a cellular automaton model
	Presently also includes the API for implementing the grid cell -- to do? move to new file?
*/

#ifndef GRID_H
#define GRID_H

#include <assert.h>	// for assert()
#include <stdlib.h> // for size_t

/*
	DATA STRUCTURES
*/

typedef struct GridCell {
	double meanTemperature;
	double minTemperature;
	void * cellData;	// will fill this with something when appropriate
};


typedef struct Grid {
	size_t 		xDim, yDim;
	GridCell * 	gridData;

};



/*
	FUNCTION PROTOTYPES
*/

Grid * 		make_grid		(size_t xsize, size_t ysize, GridType gridType);
void 		destroy_grid		(Grid * grid);
GridCell * 	grid_get		 	(Grid * grid, size_t x, size_t y);
GridCell * 	grid_set			(Grid * grid, GridCell * value, size_t x, size_t y);
GridCell **	get_neighbors		(Grid * grid, x, y)



#endif