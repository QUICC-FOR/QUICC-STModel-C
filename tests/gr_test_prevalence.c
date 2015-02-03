#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <math.h>

#include "../src/grid.c"
#include "../src/climate.c"

int main() {

	/*
		test out the computation of prevalence on a small controlled grid
	*/

	int testResult = 0;
	short globalPrevalence =0;

	// verify that st_state_to_index fails properly
	testResult += (st_state_to_index('D') != -1);

	// test both neighborhood types
	Grid * testGr = gr_make_grid(3, 3, MOORE, UNIFORM, 0, NULL);
	StateData prevalence;
	gr_compute_local_prevalence(testGr, 1, 1, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 3.0/8.0;
	testResult += prevalence[st_state_to_index('B')] != 3.0/8.0;
	testResult += prevalence[st_state_to_index('M')] != 2.0/8.0;
	testResult += prevalence[st_state_to_index('R')] != 0;

	testGr = gr_make_grid(3, 3, VONNE, UNIFORM, 0, NULL);
	gr_compute_local_prevalence(testGr, 1, 1, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 1.0/4.0;
	testResult += prevalence[st_state_to_index('B')] != 1.0/4.0;
	testResult += prevalence[st_state_to_index('M')] != 2.0/4.0;
	testResult += prevalence[st_state_to_index('R')] != 0;

	// test edges in each direction with MOORE neighborhoods, also add an R
	testGr = gr_make_grid(3, 3, MOORE, UNIFORM, 0, NULL);
	testGr->stateCurrent[1][2] = 'R';
	double epsilon = 0.0000001;
	gr_compute_local_prevalence(testGr, 2, 2, prevalence);
	testResult += prevalence[st_state_to_index('T')] != 0;
	testResult += prevalence[st_state_to_index('B')] != 0;
	testResult += fabs(prevalence[st_state_to_index('M')] - 2.0/3.0) > epsilon;
	testResult += fabs(prevalence[st_state_to_index('R')] - 1.0/3.0) > epsilon;

	gr_compute_local_prevalence(testGr, 0, 1, prevalence);
	testResult += fabs(prevalence[st_state_to_index('T')] - 2.0/5.0) > epsilon;
	testResult += fabs(prevalence[st_state_to_index('B')] - 1.0/5.0) > epsilon;
	testResult += fabs(prevalence[st_state_to_index('M')] - 1.0/5.0) > epsilon;
	testResult += fabs(prevalence[st_state_to_index('R')] - 1.0/5.0) > epsilon;
	return testResult;
}