
#include "grid.h"
#include "io.h"
#include "grid_cell.h"

#include <gsl/gsl_rng.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>


#define TIME_STEP number;

int main(int argc, char ** argv){

	// set up RNG
	gsl_rng * rng = gsl_rng_alloc(gsl_rng_mt19937);
	assert(rng);
	gsl_rng_set(rng, (int) time(NULL)); 


    make_grid();
    parse_input();
    set_grid(data);

for (y, y<TIME_STEP, y++){

    for (i, i<size_t, i++){
        update_prevalence();  /*Prevalence and state is going to change at each time step*/
        get_trans_prob();
        select_new_state();
    }

}
    process_output();


	gsl_rng_free(rng);
    return 0;

}