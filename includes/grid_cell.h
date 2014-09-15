#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <gsl/gsl_rng.h>
#include "climate.h"

/*
    DATA STRUCTURES
*/

#define GC_NUM_STATES 4
typedef enum {MIXED,DECIDUOUS,CONIFEROUS,TRANSITIONAL} State;
typedef double StateData [GC_NUM_STATES];
extern const State GC_POSSIBLE_STATES[GC_NUM_STATES];

typedef struct {
    State* currentState;
    State* stateHistory;
    Climate* climate;
    StateData prevalence;
    StateData transitionProbs;
    size_t historySize;
    size_t x;
    size_t y;
} GridCell;


/*
    FUNCTION PROTOTYPES
*/

GridCell* gc_make_cell(size_t numTimeSteps, size_t x, size_t y); // allocate memory and null initialize cell, initialize pointers
void gc_get_trans_prob(GridCell* cell);
void gc_select_next_state(GridCell* cell, gsl_rng* rng);
void gc_update_current_state(GridCell *cell);
void gc_destroy_cell(GridCell *cell);
char st_state_to_char(State s);

#endif
