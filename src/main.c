// #define NDEBUG   // enable this to switch off debugging/assertions

/*
	some performance notes
	with a ~ 1 ha cell size (15000x15000)
		mem use: ~450 MB
		running time: ~ 11.5 mins
	with 10000x10000 cells:
		mem use: ~150 MB
		running time ~ 5 mins
*/

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include <gsl/gsl_rng.h>

#include "grid.h"
#include "climate.h"

// set global modelling constants
static int MAX_TIME = 21;
static int GR_SIZE_X = 100;
static int GR_SIZE_Y = 100;
static int CLIM_X_DIM = 10;
static int CLIM_Y_DIM = 10;
static const GrNeighborhoodType NBTYPE = MOORE;
static const double DISTURB_RATE = 0.20;
static const char * climateDataFile = "climate_test.csv";

int main(int argc, char **argv) {

	for(int i = 0; i < argc; i++) {
		if(strncmp(argv[i],"-t",2)==0)
			MAX_TIME = atoi(argv[++i]);
		if(strncmp(argv[i],"-s",2)==0)
			GR_SIZE_X = GR_SIZE_Y = atoi(argv[++i]);
		if(strncmp(argv[i],"-x",2)==0)
			GR_SIZE_X = atoi(argv[++i]);
		if(strncmp(argv[i],"-y",2)==0)
			GR_SIZE_Y = atoi(argv[++i]);
	}

	// check inputs
	if((GR_SIZE_X % CLIM_X_DIM) || (GR_SIZE_Y % CLIM_Y_DIM)) {
		fprintf(stderr, "Error: grid dimensions must be evenly divisible by the dimensions of the climate grid\n");
		exit(EXIT_FAILURE);
	}

  // set up RNG
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int)time(NULL));
  
  
	ClimateGrid * climGrid = cg_make_climate_grid(climateDataFile, CLIM_X_DIM, CLIM_Y_DIM, MAX_TIME - 1);
	Grid *grid = gr_make_grid(GR_SIZE_X, GR_SIZE_Y, NBTYPE, UNIFORM, DISTURB_RATE, rng);

	// main loop in time -- we start on year 1, end on MAX_TIME - 1; this gives us
	// MAX_TIME total time steps (including the initial conditions)
	fprintf(stderr, "Starting model\n");
	for (int year = 1; year < MAX_TIME; year++) {
		for(int x = 0; x < grid->xdim; x++) {
			for(int y = 0; y < grid->ydim; y++) {
				Climate * currentClim = cg_climate_from_grid(climGrid, year-1, x, y, grid->xdim, grid->ydim);
				gr_update_cell(grid, x, y, currentClim, rng);
			}
		}
		gr_advance_state(grid);
		fprintf(stderr, "Completed year %d\n", year);
	}

	gr_output(grid);

	// cleanup
	gr_destroy_grid(grid);
	gsl_rng_free(rng);
}
