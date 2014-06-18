#ifndef GRIDCELL_H
#define GRIDCELL_H

/*
    DATA STRUCTURES
*/

typedef enum State { D,C,T,M };

typedef struct StateData{
    double D,C,T,M;
};

typedef struct Climate{
    double meanTemp;
    // Fill later
};

typedef struct GridCell{
    State* currentState;
    State* stateHistory;
    Climate climate;
    StateData prevalence;
};


/*
    FUNCTION PROTOTYPES
*/

GridCell* make_cell ();
void destroy_cell (GridCell* gridcell);
StateData get_trans_prob (GridCell* gridcell);
void select_new_state (GridCell* gridcell, StateData transprob);

#endif