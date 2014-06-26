#include "grid_cell.h"
#include <stdlib.h>

/*FUNCTION PROTOTYPES 	these are functions, not variables*/
double beta_d (Climate climate);
double beta_c (Climate climate);
double theta_c (Climate climate);
double theta_d (Climate climate);
double phi_d (Climate climate);
double phi_c (Climate climate);
double phi_m (Climate climate);
double epsi  (Climate climate);


void get_trans_prob(GridCell* cell)
{
/*FUNCTION DESCRIPTION: Parameters and ouput */


StateData * transProb; // output

    switch ( *(cell->currentState) ) {

            case TRANSITIONAL:
            	// the line below is an error, and it is repeated for each type
            	// is it not possible that a state will stay the same from one time step to the next?
                cell->transitionProbs[TRANSITIONAL] =1 - (phi_c(cell->climate)+phi_d(cell->climate)+phi_m(cell->climate)); // remember, these are functions which will depend on the climate of the cell in question
                cell->transitionProbs[CONIFEROUS] = phi_c(cell->climate);
                cell->transitionProbs[DECIDUOUS] = phi_d(cell->climate);
                cell->transitionProbs[MIXED] = phi_m(cell->climate);
                break;

            case MIXED:
                cell->transitionProbs[TRANSITIONAL] = epsi(cell->climate);
                cell->transitionProbs[CONIFEROUS] = theta_c(cell->climate);
                cell->transitionProbs[DECIDUOUS] = theta_d(cell->climate);
                cell->transitionProbs[MIXED] = 1-(epsi(cell->climate)+theta_c(cell->climate)+theta_d(cell->climate));
                break;

            case DECIDUOUS:
                cell->transitionProbs[TRANSITIONAL] = epsi(cell->climate);
                cell->transitionProbs[CONIFEROUS] = 0;
                cell->transitionProbs[DECIDUOUS] = 1 - (epsi(cell->climate)+beta_c(cell->climate)*(cell->prevalence[CONIFEROUS]+cell->prevalence[MIXED]));
                cell->transitionProbs[MIXED] = beta_c(cell->climate)*(cell->prevalence[CONIFEROUS]+cell->prevalence[MIXED]);
                break;

            case CONIFEROUS:
                cell->transitionProbs[TRANSITIONAL] = epsi(cell->climate);
                cell->transitionProbs[CONIFEROUS] = 1 - (epsi(cell->climate)+beta_d(cell->climate)*(cell->prevalence[MIXED]+cell->prevalence[DECIDUOUS]));
                cell->transitionProbs[DECIDUOUS] = 0;
                cell->transitionProbs[MIXED] = beta_d(cell->climate)*(cell->prevalence[MIXED]+cell->prevalence[DECIDUOUS]);
                break;

            default:
            	// error!
            	abort();
            	break;
    }

}



void select_new_state(GridCell* cell, gsl_rng* rng)
{
    double rValue = gsl_rng_uniform(rng);
    double testVal = cell->transitionProbs[DECIDUOUS];
    State newState;

    if(rValue < testVal)
        newState = DECIDUOUS;
    else {
        testVal += cell->transitionProbs[CONIFEROUS];
        if(rValue < testVal)
            newState = CONIFEROUS;
        else {
            testVal += cell->transitionProbs[TRANSITIONAL];
            if(rValue < testVal)
                newState = TRANSITIONAL;
            else
                newState = MIXED;
        }
    }
}

// TO IMPLEMENT
// FOR NOW, THESE RETURN A DUMMY VALUE; EVENTUALLY WILL RETURN THE RESULTS OF
// STATISTICAL MODELS
double beta_d (Climate climate) { return 0.5; }
double beta_c (Climate climate) { return 0.5; }
double theta_c (Climate climate) { return 0.5; }
double theta_d (Climate climate) { return 0.5; }
double phi_d (Climate climate) { return 0.5; }
double phi_c (Climate climate) { return 0.5; }
double phi_m (Climate climate) { return 0.5; }
double epsi  (Climate climate) { return 0.5; }

