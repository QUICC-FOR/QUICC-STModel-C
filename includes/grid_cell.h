#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <gsl/gsl_rng.h>

/*
    DATA STRUCTURES
*/


#define GC_NUM_STATES 4
typedef enum { DECIDUOUS,CONIFEROUS,TRANSITIONAL,MIXED} State;
typedef double StateData [GC_NUM_STATES];

extern State GC_POSSIBLE_STATES [GC_NUM_STATES];	// this is an array giving all possible states

typedef struct {
    double meanTemp;
    /*Task: list all variables need to be used*/
    // Fill later
} Climate;

typedef struct {
    State* currentState;
    State* stateHistory;
    Climate climate;
    StateData prevalence;
    StateData transitionProbs;
    size_t historySize;
} GridCell;


/*.
    FUNCTION PROTOTYPES
*/

GridCell* gc_make_cell (size_t numTimeSteps); // allocate memory and null initialize cell, initialize pointers
void gc_destroy_cell (GridCell* cell);
void gc_get_trans_prob (GridCell* cell);
void gc_select_new_state (GridCell* cell, gsl_rng* rng);

#endif