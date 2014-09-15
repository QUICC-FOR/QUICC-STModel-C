#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>
//#include "../src/grid_cell.c"
#include "../src/grid.c"

int main() {

	/*
		test out the computation of prevalence on a small controlled grid
	*/
	
	int testResult = 0;

	// verify that st_state_to_index fails properly
	testResult += (st_state_to_index('D') != -1);
	
	// test both neighborhood types
	Grid * testGr = gr_make_grid(3, 3, MOORE, UNIFORM, 0, NULL);
	StateData prevalence;
	gr_compute_prevalence(testGr, 1, 1, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 3.0/8.0;
	testResult += prevalence[st_state_to_index('B')] != 3.0/8.0;
	testResult += prevalence[st_state_to_index('M')] != 2.0/8.0;
	testResult += prevalence[st_state_to_index('R')] != 0;

	testGr = gr_make_grid(3, 3, VONNE, UNIFORM, 0, NULL);
	gr_compute_prevalence(testGr, 1, 1, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 1.0/4.0;
	testResult += prevalence[st_state_to_index('B')] != 1.0/4.0;
	testResult += prevalence[st_state_to_index('M')] != 2.0/4.0;
	testResult += prevalence[st_state_to_index('R')] != 0;
	
	// test torus in each direction with MOORE neighborhoods, also add an R
	testGr = gr_make_grid(3, 3, MOORE, UNIFORM, 0, NULL);
	testGr->stateCurrent[1][2] = 'R';
	gr_compute_prevalence(testGr, 2, 2, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 0;
	testResult += prevalence[st_state_to_index('B')] != 4.0/8.0;
	testResult += prevalence[st_state_to_index('M')] != 3.0/8.0;
	testResult += prevalence[st_state_to_index('R')] != 1.0/8.0;
	
	gr_compute_prevalence(testGr, 0, 0, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 5.0/8.0;
	testResult += prevalence[st_state_to_index('B')] != 0;
	testResult += prevalence[st_state_to_index('M')] != 3.0/8.0;
	testResult += prevalence[st_state_to_index('R')] != 0;

	return testResult;
}