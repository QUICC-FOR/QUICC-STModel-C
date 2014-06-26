#include "grid_cell.h"
#include <stdlib.h>

/*GLOBAL VARIABLE	these are functions, not variables*/
double beta_d (Climate climate);
double beta_c (Climate climate);
double theta_c (Climate climate);
double theta_d (Climate climate);
double phi_d (Climate climate);
double phi_c (Climate climate);
double phi_m (Climate climate);
double epsi  (Climate climate);


StateData get_trans_prob(GridCell* gridcell)
{
/*FUNCTION DESCRIPTION: Parameters and ouput */

// Need to declare input variable ????
// No, already declared in the function definition
State state = gridcell->currentState;
StateData prevalence = gridcell->prevalence;

// Prob with highlights colors....
StateData transProb; // output

    switch ( state ) {

            case TRANSITIONAL:
            	// the line below is an error, and it is repeated for each type
            	// is it not possible that a state will stay the same from one time step to the next?
                trans_prob.T =0; // remember, these are functions which will depend on the climate of the cell in question
                trans_prob.C = phi_c(gridcell->climate);
                trans_prob.D = phi_d(gridcell->climate);
                trans_prob.M = phi_m(gridcell->climate);
                break;

            case MIXED:
                trans_prob.T = epsi(gridcell->climate);
                trans_prob.C = theta_c(gridcell->climate);
                trans_prob.D = theta_d(gridcell->climate);
                trans_prob.M = 0;
                break;

            case DECIDUOUS:
                trans_prob.T = epsi(gridcell->climate);
                trans_prob.C = 0;
                trans_prob.D = 0;
                trans_prob.M = beta_c(gridcell->climate)*(prevalence.C+prevalence.M);
                break;

            case CONIFEROUS:
                trans_prob.T = epsi(gridcell->climate);
                trans_prob.C = 0;
                trans_prob.D = 0;
                trans_prob.M = beta_d(gridcell->climate)*(prevalence.M+prevalence.D);
                break;

            default:
            	// error!
            	abort();
            	break;
    }

return transProb;

}



StateData select_new_state(GridCell* gridcell)
{


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

