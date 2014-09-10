#include <assert.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"
#include "../src/grid.c"

#define  TEST_ERROR_THRESHOLD 0.0001

int main() {

    /* DESC: Coming soon */
    

    // Declare variables
    size_t histSize = 10;
    GridCell * cell = gc_make_cell(histSize);   

    // define prevalence conditions to test
    // currently testing equality, plus complete dominance by each state
    int numPrevTests = GC_NUM_STATES+1;
    StateData prevTests [numPrevTests];
    for(int i = 0; i < GC_NUM_STATES; i++) {
        prevTests[0][i] = 1.0 / GC_NUM_STATES;
        for(int j = 1; j < numPrevTests; j++) {
            prevTests[j][i] = ( j == i+1 ? 1 : 0);
                }
    }
    
    // define currentState conditions to test
    int numStateTests = GC_NUM_STATES;
    State stateTests [] = {DECIDUOUS, CONIFEROUS, MIXED, TRANSITIONAL};

    // define climate conditions to test
    // not implemented
    // double meanTempTestVals [] = {-3,-2,-1,0,1,2,3};

    // loop over prevalence conditions
    for(int prevInd = 0; prevInd < numPrevTests; prevInd++) {
        for(int i = 0; i < GC_NUM_STATES; i++) cell->prevalence[i] = prevTests[prevInd][i];
        
        // loop over state tests
        for(int stateInd = 0; stateInd < numStateTests; stateInd++) {
            cell->currentState[0] = stateTests[stateInd];
            
            // init all probs to -1 to start with, so we aren't just using old vals
            for(int j = 0; j < GC_NUM_STATES; j++)
                cell->transitionProbs[j] = -1;
    
            gc_get_trans_prob(cell);

            // check that they sum to one and that all are on [0,1]
            double sum = 0;
            for(int i = 0; i < GC_NUM_STATES; i++) {
                sum += cell->transitionProbs[i];
                assert(cell->transitionProbs[i] >= 0 || cell->transitionProbs[i] <= 1);
            }
            assert(fabs(1.0 - sum) < TEST_ERROR_THRESHOLD);
        }
    }

}
