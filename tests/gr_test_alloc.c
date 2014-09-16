#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid.c"
#include "../src/climate.c"

int main() {

    /* DESC: Test memory allocated to the grid */

    // Init random number generator
    gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);
    assert(rng);
    gsl_rng_set(rng, (int) time(NULL)); 

	Grid * testGr;
	// try square, rectangular, and some very large configurations
	unsigned int xs [] = {100, 100, 200, 20000};
	unsigned int ys [] = {100, 200, 100, 20000};
	GrStartingConditionType sc [] = {RANDOM, RANDOM, MIX, UNIFORM};
	double d [] = {0.5, 0.3, 0.1, 0};
	for(int i = 0; i < 4; i++) {
		testGr = gr_make_grid(xs[i], ys[i], MOORE, sc[i], d[i], rng);
		assert(testGr->stateCurrent && testGr->stateCurrent[xs[i]-1]);
		assert(testGr->stateNext && testGr->stateNext[xs[i]-1]);
		gr_destroy_grid(testGr);
	}
    gsl_rng_free(rng);
    return 0;
}
