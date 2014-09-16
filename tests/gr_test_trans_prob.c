#include <math.h>
#include "../src/grid.c"
#include "../src/climate.c"

#define  TEST_ERROR_THRESHOLD 0.0001

int main() {

	int testResult = 0;

    // define prevalence conditions to test
    // currently testing equality, plus complete dominance by each state
    int numPrevTests = GR_NUM_STATES+1;
    StateData prevTests [numPrevTests];
    for(int i = 0; i < GR_NUM_STATES; i++) {
        prevTests[0][i] = 1.0 / GR_NUM_STATES;
        for(int j = 1; j < numPrevTests; j++) {
            prevTests[j][i] = ( j == i+1 ? 1 : 0);
        }
    }
    
    // define currentState conditions to test
    int numStateTests = GR_NUM_STATES;
    char stateTests [] = {'T', 'B', 'M', 'R'};

    // define climate conditions to test
    // not implemented
    // double meanTempTestVals [] = {-3,-2,-1,0,1,2,3};
    Climate * clim = NULL;

    // loop over prevalence conditions
    for(int prevInd = 0; prevInd < numPrevTests; prevInd++) {
    	StateData prev;
        for(int i = 0; i < GR_NUM_STATES; i++) prev[i] = prevTests[prevInd][i];
        
        // loop over state tests
        for(int stateInd = 0; stateInd < numStateTests; stateInd++) {
            char state = stateTests[stateInd];
            StateData transitionProbs;
            // init all probs to -1 to start with, so we aren't just using old vals
            for(int j = 0; j < GR_NUM_STATES; j++) transitionProbs[j] = -1;

			st_get_trans_probs(state, prev, clim, transitionProbs);

            // check that they sum to one and that all are on [0,1]
            double sum = 0;
            for(int i = 0; i < GR_NUM_STATES; i++) {
                sum += transitionProbs[i];
                if(transitionProbs[i] < 0 || transitionProbs[i] > 1) {
                	testResult++;
	                fprintf(stderr, "Test (%d,%d), transition probs = (%f, %f, %f, %f)\n", prevInd, stateInd, transitionProbs[0],transitionProbs[1],transitionProbs[2],transitionProbs[3]);
	                fprintf(stderr, "state=%c, T=%f, B=%f, R=%f, M=%f\n", state, prev[st_state_to_index('T')],
	                	prev[st_state_to_index('B')], prev[st_state_to_index('R')], prev[st_state_to_index('M')]);
				}
            }
            testResult += (fabs(1.0 - sum) > TEST_ERROR_THRESHOLD);
        }
    }
    return testResult;
}
