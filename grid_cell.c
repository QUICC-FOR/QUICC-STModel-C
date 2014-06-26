#include "grid_cell.h"

/*GLOBAL VARIABLE*/
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
State state = GridCell*.currentState;
StateData prevalence = GridCell*.prevalence;

// Prob with highlights colors....
StateData trans_prob; // output

    switch ( state ) {
            case T:
                trans_prob.T =0;
                trans_prob.C = phi_c;
                trans_prob.D = phi_d;
                trans_prob.M = phi_m;
                break;

            case M:
                trans_prob.T = epsi;
                trans_prob.C = theta_c;
                trans_prob.D = theta_d;
                trans_prob.M = 0;
                break;

            case D:
                trans_prob.T = epsi;
                trans_prob.C = 0;
                trans_prob.D = 0;
                trans_prob.M = beta_c*(prevalence.C+prevalence.D);
                break;

            case C:
                trans_prob.T = epsi;
                trans_prob.C = 0;
                trans_prob.D = 0;
                trans_prob.M = beta_c*(prevalence.C+prevalence.D);
                break;
    }

return trans_prob;

}