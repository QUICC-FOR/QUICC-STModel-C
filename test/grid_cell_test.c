#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include "../grid_cell.h"
#include "test.h"


int gc_check_new_state(gsl_rng* rng);
int gc_check_alloc();
int gc_check_transition_probs();

int main(void) {

	// set up RNG
	gsl_rng * rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 


	int failedTests = 0;

	failedTests += gc_check_alloc();
	failedTests += gc_check_transition_probs();
	failedTests += gc_check_new_state(rng);
	
	gsl_rng_free(rng);

	test_func_print("Full Test Suite", failedTests);
	return failedTests;

}



int gc_check_new_state(gsl_rng* rng) {
	int fail = 0;
	size_t histSize = 10;
	GridCell * cell = gc_make_cell(histSize);
	*(cell->currentState) = CONIFEROUS;
	int numDraws = 100000;

	// test even trans probs, as well as total dominance by each type
	int numTransTests = NUM_STATES+1;
	StateData transTests [numTransTests];
	for(int i = 0; i < NUM_STATES; i++) {
		transTests[0][i] = 1.0 / ((double) NUM_STATES);
		for(int j = 1; j < numTransTests; j++)
			transTests[j][i] = ( j == i+1 ? 1 : 0);
	}

	// for the even condition, we check within the threshold
	StateData testCounts;
	for(int i = 0; i < NUM_STATES; i++) {
		testCounts[i] = 0;
		cell->transitionProbs[i] = transTests[0][i];	
	}
	
	for(int i = 0; i < numDraws; i++) {
		gc_select_new_state(cell, rng);
		testCounts[*(cell->currentState)]++;
		cell->currentState--;	// move it back so we don't walk off the end
	}
	
	// test proportions
	for(int i = 0; i < NUM_STATES; i++) {
		fail += (fabs((testCounts[i] / numDraws) - transTests[0][i])) > TEST_ERROR_THRESHOLD;	
	}
	
	// for complete dominance, we check absolutes - we tolerate no mistakes (1 is 1, always)
	for(int transInd = 1; transInd < numTransTests; transInd++) {
		for(int i = 0; i < NUM_STATES; i++) {
			testCounts[i] = 0;
			cell->transitionProbs[i] = transTests[transInd][i];	
		}
	
		for(int i = 0; i < numDraws; i++) {
			gc_select_new_state(cell, rng);
			testCounts[*(cell->currentState)]++;
			cell->currentState--;	// move it back so we don't walk off the end
		}

		for(int i = 0; i < NUM_STATES; i++) {
			fail += ((int) testCounts[i] != (int) (cell->transitionProbs[i] * numDraws));
		}
	}		
		

	test_func_print("    gc_check_new_state", fail);
	return(fail);
}



int gc_check_alloc() {
	// creates a cell, assigns values to it, and tries to get them back out
	int fail = 0;
	size_t histSize = 10;
	GridCell * cell = gc_make_cell(histSize);
	*(cell->currentState) = DECIDUOUS;
	fail += cell->stateHistory[0] != DECIDUOUS;
	fail += cell->historySize != histSize;
	for(int i = 1; i < histSize; i++) {
		cell->currentState++;
		*(cell->currentState) = CONIFEROUS;
	}
	fail += cell->stateHistory[histSize-1] != CONIFEROUS;
	cell->climate.meanTemp = 0;
	cell->prevalence[0] = cell->transitionProbs[0] = 0.5;
	gc_destroy_cell(cell);
	
	test_func_print("    gc_check_alloc", fail);
	return fail;
}



int gc_check_transition_probs() {
	int fail = 0;
	size_t histSize = 10;
	GridCell * cell = gc_make_cell(histSize);
	

	// define prevalence conditions to test
	// currently testing equality, plus complete dominance by each state
	int numPrevTests = NUM_STATES+1;
	StateData prevTests [numPrevTests];
	for(int i = 0; i < NUM_STATES; i++) {
		prevTests[0][i] = 1.0 / NUM_STATES;
		for(int j = 1; j < numPrevTests; j++)
			prevTests[j][i] = ( j == i+1 ? 1 : 0);
	}
	
	// define currentState conditions to test
	int numStateTests = NUM_STATES;
	State stateTests [] = {DECIDUOUS, CONIFEROUS, MIXED, TRANSITIONAL};

	// define climate conditions to test
	// not implemented
	// double meanTempTestVals [] = {-3,-2,-1,0,1,2,3};

	// loop over prevalence conditions
	for(int prevInd = 0; prevInd < numPrevTests; prevInd++) {
		for(int i = 0; i < NUM_STATES; i++) cell->prevalence[i] = prevTests[prevInd][i];
		
		// loop over state tests
		for(int stateInd = 0; stateInd < numStateTests; stateInd++) {
			*(cell->currentState) = stateTests[stateInd];
			
			// init all probs to -1 to start with, so we aren't just using old vals
			for(int j = 0; j < NUM_STATES; j++)
				cell->transitionProbs[j] = -1;
	
			gc_get_trans_prob(cell);

			// check that they sum to one and that all are on [0,1]
			double sum = 0;
			for(int i = 0; i < NUM_STATES; i++) {
				sum += cell->transitionProbs[i];
				fail += cell->transitionProbs[i] < 0 || cell->transitionProbs[i] > 1;
			}
			fail += (fabs(1.0 - sum) > TEST_ERROR_THRESHOLD);
		}
	}
	
	test_func_print("    gc_check_transition_probs", fail);
	return(fail);
}