#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"

void gc_check_new_state(gsl_rng* rng);
void gc_check_alloc();
void gc_check_transition_probs();

int main() {
	// set up RNG
	gsl_rng * rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 

	gc_check_transition_probs();
	gc_check_new_state(rng);
	
	gsl_rng_free(rng);
	return EXIT_SUCCESS;
}

void gc_check_new_state(gsl_rng* rng) {
	size_t histSize = 10;
	GridCell * cell = gc_make_cell(histSize);
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
		gc_select_new_state(cell, rng);
		testCounts[*(cell->currentState)]++;
		cell->currentState--;	// move it back so we don't walk off the end
	}
	
	// test proportions
	//for(int i = 0; i < GC_NUM_STATES; i++) {
	//	fail += (fabs((testCounts[i] / numDraws) - transTests[0][i])) > TEST_ERROR_THRESHOLD;	
	//}
	
	// for complete dominance, we check absolutes - we tolerate no mistakes (1 is 1, always)
	for(int transInd = 1; transInd < numTransTests; transInd++) {
		for(int i = 0; i < GC_NUM_STATES; i++) {
			testCounts[i] = 0;
			cell->transitionProbs[i] = transTests[transInd][i];	
		}
	
		for(int i = 0; i < numDraws; i++) {
			gc_select_new_state(cell, rng);
			testCounts[*(cell->currentState)]++;
			cell->currentState--;	// move it back so we don't walk off the end
		}

		for(int i = 0; i < GC_NUM_STATES; i++) {
			assert(((int) testCounts[i] != (int) (cell->transitionProbs[i] * numDraws)));
		}
	}		
}


void gc_check_transition_probs() {
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
				assert(cell->transitionProbs[i] < 0 || cell->transitionProbs[i] > 1);
			}
			//fail += (fabs(1.0 - sum) > TEST_ERROR_THRESHOLD);
		}
	}
}
