#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <gsl/gsl_rng.h>


/*
    DATA STRUCTURES
*/

#define NUM_STATES 4
typedef enum { DECIDUOUS,CONIFEROUS,TRANSITIONAL,MIXED } State;
typedef double StateData [NUM_STATES];

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
} GridCell;


/*.
    FUNCTION PROTOTYPES
*/

//GridCell         make_cell                           (size_t numTimeSteps); // allocate memory and null initialize cell, initialize pointers
//void                destroy_cell                       (GridCell* cell);
void gc_get_trans_prob                  (GridCell* cell);
void gc_select_new_state (GridCell* cell, gsl_rng* rng);

#endif