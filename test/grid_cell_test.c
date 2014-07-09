#include <stdio.h>
#include <math.h>
#include "../grid_cell.h"

#define TEST_ERROR_THRESHOLD 0.001
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_GREEN "\x1b[32m"

// int check_boundary_conditions();
// int check_overflow_condition();
// int check_inputs();
// int check_return_values();
int gc_check_alloc();
int gc_check_transition_probs();

void test_func_print(char funcName[], int numFailed);


int main(void) {
	int failedTests = 0;

// 	failedTests += check_boundary_conditions();
// 	failedTests += check_overflow_condition();
// 	failedTests += check_inputs();
// 	failedTests += check_return_values();

	failedTests += gc_check_alloc();
	failedTests += gc_check_transition_probs();

	test_func_print("Full Test Suite", failedTests);
	return failedTests;

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
	*(cell->currentState) = DECIDUOUS;

	size_t numClimateTests = 7;							// careful, make sure this matches the arrays below
	double meanTempTestVals [] = {-3,-2,-1,0,1,2,3};

	for(int climInd = 0; climInd < numClimateTests; climInd++) {
		cell->climate.meanTemp = meanTempTestVals[climInd];
		gc_get_trans_prob(cell);

		// check that they sum to one and that all are on [0,1]
		double sum = 0;
		for(int i = 0; i < NUM_STATES; i++) {
			sum += cell->transitionProbs[i];
			fail += cell->transitionProbs[i] < 0 || cell->transitionProbs[i] > 1;
		}
		fail += (fabs(1.0 - sum) > TEST_ERROR_THRESHOLD);
		
	}

	test_func_print("    gc_check_transition_probs", fail);
	return(fail);
}

void test_func_print(char funcName [], int numFailed) {
	if(numFailed)
		printf(ANSI_COLOR_CYAN "%s:" ANSI_COLOR_RED " failed %d tests" ANSI_COLOR_RESET "\n", funcName, numFailed);
	else
		printf(ANSI_COLOR_CYAN "%s:" ANSI_COLOR_GREEN " all tests passed" ANSI_COLOR_RESET "\n", funcName);
}
// 
// int check_boundary_conditions() {return 1;}
// int check_overflow_condition() {return 1;}
// int check_inputs() {return 1;}
// int check_return_values() {return 1;}