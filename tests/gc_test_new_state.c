#include <assert.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"
#include "../src/grid.c"

#define  TEST_ERROR_THRESHOLD 0.01


int main() {

    /* DESC: Coming soon */
    
    // set up RNG
    gsl_rng * rng = gsl_rng_alloc(gsl_rng_mt19937);
    assert(rng);
    gsl_rng_set(rng, (int) time(NULL)); 


    // declare variables
    size_t histSize = 10;
    GridCell * cell = gc_make_cell(histSize, 0, 0);
    cell->currentState[0] = CONIFEROUS;
    int numDraws = 100000;

    // test even trans probs, as well as total dominance by each type
    int numTransTests = GC_NUM_STATES+1;
    StateData transTests [numTransTests];
    for(int i = 0; i < GC_NUM_STATES; i++) {
        transTests[0][i] = 1.0 / ((double) GC_NUM_STATES);
        for(int j = 1; j < numTransTests; j++)
            transTests[j][i] = ( j == i+1 ? 1 : 0);
    }

    // for the even condition, we check within the threshold
    StateData testCounts;
    for(int i = 0; i < GC_NUM_STATES; i++) {
        testCounts[i] = 0;
        cell->transitionProbs[i] = transTests[0][i];    
    }
    
    for(int i = 0; i < numDraws; i++) {
        gc_select_next_state(cell, rng);
        // we don't care to keep the history, so just set the current state equal to the next
        *(cell->currentState) = *(cell->currentState + 1);
        testCounts[*(cell->currentState)]++;
    }
    
    // test proportions
    for(int i = 0; i < GC_NUM_STATES; i++) {
      assert(fabs((testCounts[i] / numDraws) - transTests[0][i]) < TEST_ERROR_THRESHOLD);    
    }
    
    // for complete dominance, we check absolutes - we tolerate no mistakes (1 is 1, always)
    for(int transInd = 1; transInd < numTransTests; transInd++) {
        for(int i = 0; i < GC_NUM_STATES; i++) {
            testCounts[i] = 0;
            cell->transitionProbs[i] = transTests[transInd][i]; 
        }
    
        for(int i = 0; i < numDraws; i++) {
	        gc_select_next_state(cell, rng);
	        // we don't care to keep the history, so just set the current state equal to the next
	        *(cell->currentState) = *(cell->currentState + 1);
	        testCounts[*(cell->currentState)]++;
        }

        for(int i = 0; i < GC_NUM_STATES; i++) {
            assert((int) testCounts[i] == (int) (cell->transitionProbs[i] * numDraws));
        }
    }       

}
