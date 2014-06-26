#ifndef GRIDCELL_H
#define GRIDCELL_H


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

//GridCell*         make_cell                           ();
//GridCell*         update_cell                        ();
//void                destroy_cell                       (GridCell* cell);
void       get_trans_prob                  (GridCell* cell);
//void                select_new_state               (GridCell* cell, StateData transprob, gsl_rng* rng);

#endif