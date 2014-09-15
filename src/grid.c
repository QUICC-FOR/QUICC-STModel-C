#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include "grid.h"

const char GR_POSSIBLE_STATES [GR_NUM_STATES] = {'T', 'B', 'R', 'M'};

// #include "grid_cell.h"

/* FUNCTION PROTOTYPES */
static inline void gr_compute_prevalence(Grid *grid, int x, int y, StateData prevalence) ;
static void gr_set_random_grid(Grid *grid, gsl_rng *rng);
static void gr_set_uniform_grid(Grid *grid);
static void gr_set_mixed_grid(Grid *grid, gsl_rng *rng);
static void gr_disturb_grid(Grid *grid, double distRate, gsl_rng *rng);
static inline OffsetType * gr_get_nb_offsets(unsigned short nbSize);
static inline void ot_set(OffsetType * o, short x, short y);
static inline int st_state_to_index(char state);
static inline void st_get_trans_probs(char state, StateData prevalence, Climate * clim, StateData destProbs);
static inline char st_select_state(StateData transProbs, gsl_rng * rng);

// prototypes for functions defining model parameters
static inline double beta_t(Climate *climate);
static inline double beta_b(Climate *climate);
static inline double theta_b(Climate *climate);
static inline double theta_t(Climate *climate);
static inline double alpha_t(Climate *climate);
static inline double alpha_b(Climate *climate);
static inline double epsi_m(Climate *climate);
static inline double epsi_b(Climate *climate);
static inline double epsi_t(Climate *climate);



void gr_update_cell(Grid * grid, int x, int y, Climate * currClimate, gsl_rng *rng)
/*
	Current climate is a pointer to a climate struct giving the climate for this cell
*/
{
	StateData prevalence;
	gr_compute_prevalence(grid, x, y, prevalence);
	StateData transitionProbs;
	st_get_trans_probs(grid->stateCurrent[x][y], prevalence, currClimate, transitionProbs);
	grid->stateNext[x][y] = st_select_state(transitionProbs, rng);
}

void gr_advance_state(Grid * gr)
/*
	swaps the state pointers, effectively making what was formerly the next state now the current state
*/
{
	char ** stateSwap = gr->stateCurrent;
	gr->stateCurrent = gr->stateNext;
	gr->stateNext = stateSwap;
}

static inline void st_get_trans_probs(char state, StateData prevalence, Climate * clim, StateData destProbs)
{
  /*
          calculate the transition probabilities given the current state
  */
  
	// create prevalence aliases to make the math more readable
	double M = prevalence[st_state_to_index('M')];
	double B = prevalence[st_state_to_index('B')];
	double T = prevalence[st_state_to_index('T')];
	
	switch(state) {
	case 'R':
		destProbs[st_state_to_index('T')] = alpha_t(clim) * (M+T) * (1.0 - alpha_b(clim) * (M+B));
		destProbs[st_state_to_index('B')] = alpha_b(clim) * (M+B) * (1.0 - alpha_t(clim) * (M+T));
		destProbs[st_state_to_index('M')] = alpha_t(clim) * (M+B) * alpha_b(clim) * (M+B);
		destProbs[st_state_to_index('R')] = 1 - (destProbs[st_state_to_index('B')] + 
			destProbs[st_state_to_index('M')] + destProbs[st_state_to_index('T')]);
    	break;

	case 'M':    
		destProbs[st_state_to_index('R')] = epsi_m(clim);
		destProbs[st_state_to_index('B')] = theta_b(clim);
		destProbs[st_state_to_index('T')] = theta_t(clim);
		destProbs[st_state_to_index('M')] = 1 - (destProbs[st_state_to_index('B')] + 
			destProbs[st_state_to_index('R')] + destProbs[st_state_to_index('T')]);
    	break;

	case 'T':    
		destProbs[st_state_to_index('R')] = epsi_t(clim);
		destProbs[st_state_to_index('M')] = beta_b(clim) * (B+M);
		destProbs[st_state_to_index('B')] = 0;
		destProbs[st_state_to_index('T')] = 1 - (destProbs[st_state_to_index('B')] + 
			destProbs[st_state_to_index('R')] + destProbs[st_state_to_index('M')]);
    	break;

	case 'B':    
		destProbs[st_state_to_index('R')] = epsi_b(clim);
		destProbs[st_state_to_index('T')] = 0;
		destProbs[st_state_to_index('M')] = beta_t(clim) * (T+M);
		destProbs[st_state_to_index('B')] = 1 - (destProbs[st_state_to_index('T')] + 
			destProbs[st_state_to_index('R')] + destProbs[st_state_to_index('M')]);
    	break;
	}
}


static inline char st_select_state(StateData transProbs, gsl_rng * rng)
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


 
static inline void gr_compute_prevalence(Grid *grid, int x, int y, StateData prevalence) 
{
	for(int i = 0; i < GR_NUM_STATES; i++) prevalence[i] = 0;
	double increment = 1.0 / grid->nbsize;

	for(int i = 0; i < grid->nbsize; i++) {
		int newx = x + grid->nbOffsets[i].x;
		int newy = y + grid->nbOffsets[i].y;
		// set up torus; if we go out of bounds in the y direction we assume the type is
		// temperate if we are in the south and boreal if in the north
		char neighborState;
		if(newy < 0) {
			neighborState = 'T';
		} else if (newy >= grid->ydim) {
			neighborState = 'B';
		} else {
			// for x, we just wrap around
			if(newx < 0) {
				newx = grid->xdim - 1;
			} else if(newx >= grid->xdim) {
				newx = 0;
			}
			neighborState = grid->stateCurrent[newx][newy];
		}
		prevalence[st_state_to_index(neighborState)] += increment;
	}
}




static inline int st_state_to_index(char state)
{
	// on error, this will return -1, allowing you to check for invalid states (or crash)
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
	fprintf(stderr, "Building grid...\n");
	// check for runtime errors
	if(xsize > GR_MAX_DIM_SIZE || ysize > GR_MAX_DIM_SIZE) {
		fprintf(stderr, "Error allocating grid; one dimension is greater than the maximum allowable dimension of %d\n", GR_MAX_DIM_SIZE);
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

// TO IMPLEMENT
// FOR NOW, THESE RETURN A DUMMY VALUE; EVENTUALLY WILL RETURN THE RESULTS OF
// STATISTICAL MODELS
static inline double beta_t(Climate *climate) {
  return 0.5;
}
static inline double beta_b(Climate *climate) {
  return 0.5;
}
static inline double theta_b(Climate *climate) {
  return 0.25;
}
static inline double theta_t(Climate *climate) {
  return 0.25;
}
static inline double alpha_t(Climate *climate) {
  return 0.2;
}
static inline double alpha_b(Climate *climate) {
  return 0.2;
}
static inline double epsi_m(Climate *climate) {
  return 0.01;
}
static inline double epsi_b(Climate *climate) {
  return 0.01;
}
static inline double epsi_t(Climate *climate) {
  return 0.01;
}

