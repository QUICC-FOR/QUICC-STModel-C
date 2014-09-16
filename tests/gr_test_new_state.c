#include <assert.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>

#include "../src/grid.c"
#include "../src/climate.c"

#define  TEST_ERROR_THRESHOLD 0.01


int main() {

    gsl_rng * rng = gsl_rng_alloc(gsl_rng_mt19937);
    assert(rng);
    gsl_rng_set(rng, (int) time(NULL)); 
	int testResult = 0;

    int numDraws = 100000;

    // test even trans probs, as well as total dominance by each type
    int numTransTests = GR_NUM_STATES+1;
    StateData transTests [numTransTests];
    for(int i = 0; i < GR_NUM_STATES; i++) {
        transTests[0][i] = 1.0 / ((double) GR_NUM_STATES);
        for(int j = 1; j < numTransTests; j++)
            transTests[j][i] = ( j == i+1 ? 1 : 0);
    }

	// check the even transition probabilities (i.e., all states equally represented
	// use the error threshold to decide if the test passes

	StateData result;
    for(int i = 0; i < GR_NUM_STATES; i++) result[i] = 0;
    
    for(int i = 0; i < numDraws; i++) {
		char newState = st_select_state(transTests[0], rng);
		result[st_state_to_index(newState)] += 1.0 / numDraws;
    }
    
    // test proportions
    for(int i = 0; i < GR_NUM_STATES; i++) {
		if(fabs(result[i] - transTests[0][i]) > TEST_ERROR_THRESHOLD) {
			fprintf(stderr, "New state selection outside of threshold:\n");
			fprintf(stderr, "T=%f; B=%f, M=%f, R=%f\n", result[st_state_to_index('T')], result[st_state_to_index('B')], result[st_state_to_index('M')], result[st_state_to_index('R')]);
			testResult++;
		}
    }
    
    // for complete dominance, we check absolutes - we tolerate no mistakes (1 is 1, always)
    for(int transInd = 1; transInd < numTransTests; transInd++) {
        for(int i = 0; i < GR_NUM_STATES; i++) result[i] = 0;

		for(int i = 0; i < numDraws; i++) {
			char newState = st_select_state(transTests[transInd], rng);
			result[st_state_to_index(newState)]++;
		}

	    for(int i = 0; i < GR_NUM_STATES; i++)
	    	testResult += (int) result[i] != (int) transTests[transInd][i] * (int) numDraws;
	}       
	return testResult;
}
