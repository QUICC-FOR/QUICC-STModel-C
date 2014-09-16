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
#include <unistd.h>

#include <gsl/gsl_rng.h>

#include "grid.h"
#include "climate.h"

static void parse_args(int argc, char **argv);
static void help();


// set global modelling variables; these can be set using command line options
static int MAX_TIME = 21;
static int GR_SIZE_X = 100;
static int GR_SIZE_Y = 100;
static int CLIM_X_DIM = 10;
static int CLIM_Y_DIM = 10;
static GrNeighborhoodType NBTYPE = MOORE;
static float DISTURB_RATE = 0.20;
static char * climateDataFile = "climate_test.csv";
static char * gridDataFile = "";
static short gridFromFile = 0;

int main(int argc, char **argv) {

	parse_args(argc, argv);

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
	Grid *grid;
	if(gridFromFile)
		grid = grid_from_file(GR_SIZE_X, GR_SIZE_Y, NBTYPE, gridDataFile);
	else
		grid = gr_make_grid(GR_SIZE_X, GR_SIZE_Y, NBTYPE, UNIFORM, DISTURB_RATE, rng);

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

static void parse_args(int argc, char ** argv)
{
	char * options = "hvd:t:x:y:a:b:c:g:";
	char opt;
	int error = 0;
	while( (opt = getopt(argc, argv, options)) != -1) {
		switch(opt) {
		case 'h':
			help();
			break;
		case ':':
			error++;
			break;		
		case '?':
			error++;
			break;
 		case 't':
 			MAX_TIME = strtol(optarg, NULL, 0) + 1;
 			break;
 		case 'x':
			GR_SIZE_X = strtol(optarg, NULL, 0);
			break;
		case 'y':
			GR_SIZE_Y = strtol(optarg, NULL, 0);
			break;
		case 'a':
			CLIM_X_DIM = strtol(optarg, NULL, 0);
			break;
		case 'b':
			CLIM_Y_DIM = strtol(optarg, NULL, 0);
			break;
		case 'c':
			climateDataFile = optarg;
			break;
		case 'g':
			gridDataFile = optarg;
			gridFromFile = 1;
			break;
		case 'v':
			NBTYPE = VONNE;
			break;
		case 'd':
			DISTURB_RATE = strtof(optarg,NULL);
			break;
		}
	if(error) exit(EXIT_FAILURE);
	}
}


static void help()
{
	fprintf(stderr, "Optional command line options. Default values shown in parentheses:\n");
	fprintf(stderr, "  -h: help; display this message\n");
	fprintf(stderr, "  -x <int>: specify x dimension of the simulation grid; must divide evenly into x-dim of climate grid (%d)\n", GR_SIZE_X);
	fprintf(stderr, "  -y <int>: specify y dimension of the simulation grid; must divide evenly into x-dim of climate grid (%d)\n", GR_SIZE_Y);
	fprintf(stderr, "  -a <int>: specify x dimension of the climate grid (%d)\n", CLIM_X_DIM);
	fprintf(stderr, "  -b <int>: specify y dimension of the climate grid (%d)\n", CLIM_Y_DIM);	
	fprintf(stderr, "  -c <filename>: specify the input climate datafile; must match values in -a, -b, and -t options (%s)\n", climateDataFile);	
	fprintf(stderr, "  -g <filename>: use a previously output grid as input, rather than initializing a new grid (unset)\n");	
	fprintf(stderr, "  -t <int>: specify the number of time steps after after initial conditions to run the simulation (%d)\n", MAX_TIME-1);
	fprintf(stderr, "  -d <float>: specify initial disturbance rate between 0 and %f (%f)\n", GR_MAX_DISTURBANCE_RATE, DISTURB_RATE);
	fprintf(stderr, "  -v: specify Von Neuman neighborhoods (4-cell; default is 8-cell Moore neighborhood)\n");
	
	exit(EXIT_SUCCESS);
}

































