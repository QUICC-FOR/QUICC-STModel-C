#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>

#include "grid.h"

const char GR_POSSIBLE_STATES [GR_NUM_STATES] = {'T', 'B', 'R', 'M'};

// #include "grid_cell.h"

/* FUNCTION PROTOTYPES */
static inline void gr_compute_local_prevalence(Grid *grid, int x, int y, StateData prevalence) ;
static inline void gr_compute_global_prevalence(Grid *grid, int x, int y, StateData prevalence, gsl_rng *rng) ;
static inline void gr_set_null_grid(Grid * gr);
static void gr_set_random_grid(Grid *grid, gsl_rng *rng);
static void gr_set_uniform_grid(Grid *grid);
static void gr_set_mixed_grid(Grid *grid, gsl_rng *rng);
static void gr_disturb_grid(Grid *grid, double distRate, gsl_rng *rng);
static inline OffsetType * gr_get_nb_offsets(unsigned short nbSize);
static inline void ot_set(OffsetType * o, short x, short y);
static inline int st_state_to_index(char state);
static inline void st_get_trans_probs(char state, StateData prevalence, Climate * clim, ClimatePars * climPar, StateData destProbs);
static inline char st_select_state(StateData transProbs, gsl_rng * rng);
static inline int gr_check_grid(Grid * grid);
static inline long double inv_logit(long double val);

// prototypes for functions defining model parameters
static inline double beta_t(Climate *cl, ClimatePars *cp);
static inline double beta_b(Climate *cl, ClimatePars *cp);
static inline double theta_b(Climate *cl, ClimatePars *cp);
static inline double theta_t(Climate *cl, ClimatePars *cp);
static inline double alpha_t(Climate *cl, ClimatePars *cp);
static inline double alpha_b(Climate *cl, ClimatePars *cp);
static inline double epsi(Climate *cl, ClimatePars *cp);
static inline double epsi_m(Climate *cl, ClimatePars *cp);
static inline double epsi_b(Climate *cl, ClimatePars *cp);
static inline double epsi_t(Climate *cl, ClimatePars *cp);



Grid * grid_from_file(unsigned int xsize, unsigned int ysize, GrNeighborhoodType nbType, const char * gridDataFile)
{
	// first create the grid in memory
	Grid * newGrid = gr_make_grid(xsize, ysize, nbType, GR_NULL, 0, NULL);
	fprintf(stderr, "Reading grid state from file <%s>...\n", gridDataFile);

	// borrow some code functions from climate.h for reading in the data
	FILE * iFile = fopen(gridDataFile, "r");
	if(!iFile) {
		fprintf(stderr, "Cannot open file %s\n", gridDataFile);
		exit(EXIT_FAILURE);
	}
	char line [MAX_LINE_LEN+1];
	int lineno = 0;
	readline(iFile, line);
	lineno++;
	while(line[0]) {
		char * cell = strtok(line, ",;");
		int x = strtol(cell, NULL, 0);
		cell = strtok(NULL, ",;");
		int y = strtol(cell, NULL, 0);
		cell = strtok(NULL, ",;");
		char st = cell[0];

		if(x < 0 || x >= newGrid->xdim || y < 0 || y >= newGrid->ydim) {
			fprintf(stderr, "Error: coordinates (%d,%d) are invalid for grid dimensions of (%d,%d)\n", x, y, newGrid->xdim, newGrid->ydim);
			exit(EXIT_FAILURE);
		}
		newGrid->stateCurrent[x][y] = st;
		readline(iFile, line);
		lineno++;
	}
	fclose(iFile);
	if(gr_check_grid(newGrid)) {
		fprintf(stderr, "Problem reading from the input file\n");
		exit(EXIT_FAILURE);
	}

	return newGrid;
}


void gr_update_cell(Grid * grid, int x, int y, Climate * currClimate, ClimatePars * climPars, gsl_rng *rng, const short globalPrevalence)
/*
	Current climate is a pointer to a climate struct giving the climate for this cell
*/
{
	if(grid->stateCurrent[x][y] == GR_NULL_STATE) {
		grid->stateNext[x][y] = GR_NULL_STATE;
	}
	else {
		StateData prevalence;

		if(globalPrevalence == 1) {
			gr_compute_global_prevalence(grid, x, y, prevalence,rng);
		}
		else {
			gr_compute_local_prevalence(grid, x, y, prevalence);
		}

		StateData transitionProbs;
		st_get_trans_probs(grid->stateCurrent[x][y], prevalence, currClimate, climPars, transitionProbs);
		grid->stateNext[x][y] = st_select_state(transitionProbs, rng);
	}
}

void gr_advance_state(Grid * gr)
/*
	swaps the state pointers, making what was formerly the next state now the current state
*/
{
	char ** stateSwap = gr->stateCurrent;
	gr->stateCurrent = gr->stateNext;
	gr->stateNext = stateSwap;
}

static inline void st_get_trans_probs(char state, StateData prevalence, Climate * clim, ClimatePars * climPar, StateData destProbs)
{
  /*
          calculate the transition probabilities given the current state
          Not null safe; any calling functions should check for null state beforehand
  */

	// create prevalence aliases to make the math more readable
	double M = prevalence[st_state_to_index('M')];
	double B = prevalence[st_state_to_index('B')];
	double T = prevalence[st_state_to_index('T')];

	switch(state) {
	case 'R':
		destProbs[st_state_to_index('T')] = alpha_t(clim, climPar) * (M+T) * (1.0 - alpha_b(clim, climPar) * (M+B));
		destProbs[st_state_to_index('B')] = alpha_b(clim, climPar) * (M+B) * (1.0 - alpha_t(clim, climPar) * (M+T));
		destProbs[st_state_to_index('M')] = alpha_t(clim, climPar) * (M+B) * alpha_b(clim, climPar) * (M+B);
		destProbs[st_state_to_index('R')] = 1 - (destProbs[st_state_to_index('B')] +
			destProbs[st_state_to_index('M')] + destProbs[st_state_to_index('T')]);
    	break;

	case 'M':
		destProbs[st_state_to_index('R')] = epsi_m(clim, climPar);
		destProbs[st_state_to_index('B')] = theta_b(clim, climPar);
		destProbs[st_state_to_index('T')] = theta_t(clim, climPar);
		destProbs[st_state_to_index('M')] = 1 - (destProbs[st_state_to_index('B')] +
			destProbs[st_state_to_index('R')] + destProbs[st_state_to_index('T')]);
    	break;

	case 'T':
		destProbs[st_state_to_index('R')] = epsi_t(clim, climPar);
		destProbs[st_state_to_index('M')] = beta_b(clim, climPar) * (B+M);
		destProbs[st_state_to_index('B')] = 0;
		destProbs[st_state_to_index('T')] = 1 - (destProbs[st_state_to_index('B')] +
			destProbs[st_state_to_index('R')] + destProbs[st_state_to_index('M')]);
    	break;

	case 'B':
		destProbs[st_state_to_index('R')] = epsi_b(clim, climPar);
		destProbs[st_state_to_index('T')] = 0;
		destProbs[st_state_to_index('M')] = beta_t(clim, climPar) * (T+M);
		destProbs[st_state_to_index('B')] = 1 - (destProbs[st_state_to_index('T')] +
			destProbs[st_state_to_index('R')] + destProbs[st_state_to_index('M')]);
    	break;
	}
}


static inline char st_select_state(StateData transProbs, gsl_rng * rng)
/*
	Stochastic portion of the model; randomly selects a new state give the transition
	probabilities.
	Not null safe; any calling functions should check for null state beforehand
*/
{
	double rValue = gsl_rng_uniform(rng);
	double testVal = 0;
	char newState = 0;

	for(int i = 0; i < GR_NUM_STATES; i++) {
		char curState = GR_POSSIBLE_STATES[i];
		testVal += transProbs[st_state_to_index(curState)];
		if (rValue < testVal) {
			newState = curState;
			break;
		}
	}
	return newState;
}



static inline void gr_compute_local_prevalence(Grid *grid, int x, int y, StateData prevalence)
/*
	find the neighbors of the cell at x,y and compute prevalence, storing it in
	the prevalence variable
	Note that prevalence is a StateData type, which is an array, so the underlying
	data are modifiable in the calling scope and will be preserved on return

	This function handles edge detection (edges are damped)
	This function checks for null-state neighbors, which are counted in the total number
	of neighbors but which contribute nothing to prevalence
*/
{
	for(int i = 0; i < GR_NUM_STATES; i++) prevalence[i] = 0;
	char nbStates [grid->nbsize];
	int actualNBSize = 0;

	for(int i = 0; i < grid->nbsize; i++) {
		int newx = x + grid->nbOffsets[i].x;
		int newy = y + grid->nbOffsets[i].y;
		/* check for boundaries -- we are using damped edges
			 if on an edge, you just have fewer neighbors
			 if not on an edge, append the neighbor's state (even if null) to the array
			 and increment the number of neighbors */
		if(!(newy < 0 || newx < 0 || newy >= grid->ydim || newx >= grid->xdim)) {
			nbStates[actualNBSize++] = grid->stateCurrent[newx][newy];
		}
	}
	double increment = 1.0 / actualNBSize;
	for(int i = 0; i < actualNBSize; i++) {
		if(nbStates[i] != GR_NULL_STATE)
			prevalence[st_state_to_index(nbStates[i])] += increment;
	}
}

static inline void gr_compute_global_prevalence(Grid *grid, int x, int y, StateData prevalence, gsl_rng *rng)
/*
	pick random neighbors in the grid for the cell at x,y and compute the states prevalence, storing it in
	the prevalence variable.

	NOTE ENHANCEMENT: This function need to NOT pickup a null state (see with Matt)
*/
{
	for(int i = 0; i < GR_NUM_STATES; i++) prevalence[i] = 0;
	char nbStates [grid->nbsize];


	for(int i = 0; i < grid->nbsize; i++) {
		int newx = gsl_rng_uniform_int(rng, grid->xdim);
		int newy = gsl_rng_uniform_int(rng, grid->ydim);

		nbStates[i] = grid->stateCurrent[newx][newy];
	}

	double increment = 1.0 / grid->nbsize;
	for(int i = 0; i < grid->nbsize; i++) {
		if(nbStates[i] != GR_NULL_STATE)
			prevalence[st_state_to_index(nbStates[i])] += increment;
	}
}





static inline int st_state_to_index(char state)
{
	// on error, this will return -1, allowing you to check for invalid states (or crash)
	// not null-safe - will return -1 on getting a null state; if this is undesired
	// check for null state in the calling scope
	switch(state) {
	case 'B':
		return 0;
	case 'M':
		return 1;
	case 'T':
		return 2;
	case 'R':
		return 3;
	default:
		return -1;
	}
}


Grid * gr_make_grid(unsigned int xsize, unsigned int ysize, GrNeighborhoodType nbType,
	GrStartingConditionType startingCondition, double startDisturbanceRate, gsl_rng *rng)
{
	fprintf(stderr, "Initializing (%d x %d) grid...\n", xsize, ysize);
	// check for runtime errors
	if(xsize > GR_MAX_DIM_SIZE || ysize > GR_MAX_DIM_SIZE) {
		fprintf(stderr, "Error allocating grid; a dimension is greater than the maximum allowable dimension of %d\n", GR_MAX_DIM_SIZE);
		exit(EXIT_FAILURE);
	}
	if(startDisturbanceRate < 0 || startDisturbanceRate > GR_MAX_DISTURBANCE_RATE) {
		fprintf(stderr, "Error creating grid; disturbance rate must be >= 0 and < %f\n", GR_MAX_DISTURBANCE_RATE);
		exit(EXIT_FAILURE);
	}

	Grid *newGrid = malloc(sizeof(*newGrid));

	newGrid->xdim = xsize;
	newGrid->ydim = ysize;
	newGrid->nbsize = (nbType == MOORE ? 8 : 4);
	newGrid->nbOffsets = gr_get_nb_offsets(newGrid->nbsize);

	newGrid->stateCurrent = malloc(newGrid->xdim * sizeof(*(newGrid->stateCurrent)));
	newGrid->stateNext = malloc(newGrid->xdim * sizeof(*(newGrid->stateNext)));
	for(int x = 0; x < newGrid->xdim; x++) {
		newGrid->stateCurrent[x] = malloc(newGrid->ydim * sizeof(*(newGrid->stateCurrent[x])));
		newGrid->stateNext[x] = malloc(newGrid->ydim * sizeof(*(newGrid->stateNext[x])));
	}

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

	case GR_NULL:
		gr_set_null_grid(newGrid);
		break;
	}
	gr_disturb_grid(newGrid, startDisturbanceRate, rng);
	return newGrid;
}


void gr_destroy_grid(Grid *grid)
{
	for(int x = 0; x < grid->xdim; x++) {
		free(grid->stateCurrent[x]);
		free(grid->stateNext[x]);
	}

	free(grid->nbOffsets);
	free(grid->stateCurrent);
	free(grid->stateNext);
	free(grid);
}




/*    GRID INITIALIZATION FUNCTIONS   */
static inline void gr_set_null_grid(Grid * gr)
{
	for(int x = 0; x < gr->xdim; x++) {
		for(int y = 0; y < gr->ydim; y++) {
			gr->stateCurrent[x][y] = GR_NULL_STATE;
		}
	}
}

static void gr_set_random_grid(Grid *grid, gsl_rng *rng)
{
	char allowedStates [] = {'B', 'T', 'M'};
	for(int x = 0; x < grid->xdim; x++) {
		for(int y = 0; y < grid->ydim; y++) {
			gsl_ran_choose(rng, &(grid->stateCurrent[x][y]), 1, allowedStates, 3, sizeof(*allowedStates));
		}
	}
}



static void gr_set_uniform_grid(Grid *grid)
{
	assert(grid->xdim >= 3);
	assert(grid->ydim >= 3);
	// set latitudinal thresholds for each state
	unsigned short temperateUpperLimit = grid->ydim / 3; // from 0 to 1/3
	unsigned short borealLowerLimit = (grid->ydim * 2) / 3; // from 2/3 to max

	for(int x = 0; x < grid->xdim; x++) {
		for(int y = 0; y < temperateUpperLimit; y++)
			grid->stateCurrent[x][y] = 'T';
		for(int y = temperateUpperLimit; y < borealLowerLimit; y++)
			grid->stateCurrent[x][y] = 'M';
		for(int y = borealLowerLimit; y < grid->ydim; y++)
			grid->stateCurrent[x][y] = 'B';
	}
}




static void gr_set_mixed_grid(Grid *grid, gsl_rng *rng)
{
	assert(grid->xdim >= 5);
	assert(grid->ydim >= 5);
	char states [] = {'M', 'M'};
	int yIncrement = grid->ydim / 5;

	for(int x = 0; x < grid->xdim; x++) {
		for(int y = 0; y < yIncrement; y++)
			grid->stateCurrent[x][y] = 'T';
		states[0] = 'T';
		for(int y = yIncrement; y < 2*yIncrement; y++)
			gsl_ran_choose(rng, &(grid->stateCurrent[x][y]), 1, states, 2, sizeof(*states));
		for(int y = 2*yIncrement; y < 3*yIncrement; y++)
			grid->stateCurrent[x][y] = 'M';
		states[0] = 'B';
		for(int y = 3*yIncrement; y < 4*yIncrement; y++)
			gsl_ran_choose(rng, &(grid->stateCurrent[x][y]), 1, states, 2, sizeof(*states));
		for(int y = 4*yIncrement; y < grid->ydim; y++)
			grid->stateCurrent[x][y] = 'B';
	}
}




static void gr_disturb_grid(Grid *grid, double disturbanceRate, gsl_rng *rng)
{
	if(disturbanceRate == 0)
		return;

	unsigned short numToDisturb = grid->xdim * grid->ydim * disturbanceRate;
	size_t numberDisturbed = 0;
	while(numberDisturbed < numToDisturb) {
		int x = gsl_rng_uniform_int(rng, grid->xdim);
		int y = gsl_rng_uniform_int(rng, grid->ydim);
		if(grid->stateCurrent[x][y] != 'R') {
			grid->stateCurrent[x][y] = 'R';
			numberDisturbed++;
		}
	}
}




void gr_output(Grid *grid){
	for(int x = 0; x < grid->xdim; x++) {
		for(int y = 0; y < grid->ydim; y++) {
	        printf("%d,%d,%c\n",x,y,grid->stateCurrent[x][y]);
	    }
	}
}




void gr_view_grid(Grid *grid) {
	for(int y = grid->ydim - 1; y >= 0; y--) {
		for(int x = 0; x < grid->xdim; x++)
			fprintf(stderr,"| %c ", grid->stateCurrent[x][y]);
	    fprintf(stderr,"|   %d\n", y);
	}
}


void gr_set_null(Grid *grid, int x, int y)
/// simple utility function to set a given cell to the null state
{
	grid->stateCurrent[x][y] = GR_NULL_STATE;
}


static inline int gr_check_grid(Grid * grid)
{
	int error = 0;
	for(int x = 0; x < grid->xdim; x++) {
		for(int y = 0; y < grid->ydim; y++) {
			char st = grid->stateCurrent[x][y];
			int found = 0;
			for(int i = 0; i < GR_NUM_STATES && !found; i++) {
				char checkState = GR_POSSIBLE_STATES[i];
				found = (checkState == st || st == GR_NULL_STATE);
			}
			if(!found) {
				error++;
				fprintf(stderr, "Grid has a problem at (%d,%d); state [%c] is invalid\n", x, y, st);
			}
		}
	}
	return error;
}


static inline long double inv_logit(long double val)
{
	if(val > 0)
		return 1.0 / (1.0 + exp(-val));
	else
		return exp(val) / (1.0 + exp(val));
}


static inline OffsetType * gr_get_nb_offsets(unsigned short nbSize)
{
	OffsetType * offsets = malloc(nbSize * sizeof(*offsets));
	ot_set(&(offsets[0]),-1,0);
	ot_set(&(offsets[1]),1,0);
	ot_set(&(offsets[2]),0,-1);
	ot_set(&(offsets[3]),0,1);

	if(nbSize == 8) {
		ot_set(&(offsets[4]),-1,-1);
		ot_set(&(offsets[5]),-1,1);
		ot_set(&(offsets[6]),1,-1);
		ot_set(&(offsets[7]),1,1);
	}
	return offsets;
}



static inline void ot_set(OffsetType * o, short x, short y)
{
	o->x = x;
	o->y = y;
}


static inline double logit_cubic_climate_parameter(Climate * cl, double * pars)
{
	double result = pars[0] + pars[1]*cl->env1 + pars[2]*cl->env2 + pars[3]*pow(cl->env1,2) + pars[4]*pow(cl->env2,2) + pars[5]*pow(cl->env1,3) + pars[6]*pow(cl->env2,3);
	return result;
}

static inline double alpha_b(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->alphaB));
}

static inline double alpha_t(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->alphaT));
}

static inline double beta_b(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->betaB));
}

static inline double beta_t(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->betaT));
}

static inline double theta_b(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->thetaB));
}

static inline double theta_t(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->thetaT));
}

static inline double epsi(Climate *cl, ClimatePars *cp)
{
	return inv_logit(logit_cubic_climate_parameter(cl, cp->epsi));
//	return 0.01;
}

static inline double epsi_m(Climate *cl, ClimatePars *cp)
{
	return epsi(cl, cp);
}

static inline double epsi_b(Climate *cl, ClimatePars *cp)
{
	return epsi(cl, cp);
}

static inline double epsi_t(Climate *cl, ClimatePars *cp)
{
	return epsi(cl, cp);
}