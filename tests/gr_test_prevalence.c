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


	// test global prevalence

	testGr = gr_make_grid(180, 180, MOORE, UNIFORM, 0, NULL);

  	// set up RNG
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);

	// Declare global prevalence and init on 0
	StateData globalPrevalence;
	for(int i = 0; i < GR_NUM_STATES; i++) globalPrevalence[i] = 0;


	// Compute prevalence on all cells
	for(int x = 0 ; x < testGr->xdim; x++){
		for(int y = 0 ; y < testGr->ydim; y++){

			//printf("Cell %d,%d \n",x,y);
			gr_compute_global_prevalence(testGr,x,y,prevalence,rng);

			for(int i = 0; i < GR_NUM_STATES; i++) {
				globalPrevalence[i] += prevalence[i]/(testGr->ydim*testGr->xdim);
				//printf("GlobPrev of %d: %f \n",i,globalPrevalence[i]);
			}
		}
	}

	epsilon=0.005;

	testResult += fabs(prevalence[st_state_to_index('T')] - 0.33) < epsilon;
	testResult += fabs(prevalence[st_state_to_index('B')] - 0.33) < epsilon;
	testResult += fabs(prevalence[st_state_to_index('M')] - 0.33) < epsilon;


	gsl_rng_free(rng);

	return testResult;
}