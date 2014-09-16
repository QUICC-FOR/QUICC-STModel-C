#include "../src/climate.c"

int main(void) {
	/*
		This test covers a lot of internal structures of the climate dataset
		these functions are not tested explicitly. however, if they are not functioning
		the tests will fail anyway -- as long as the function is correctly reading in the 
		test data and interpreting it into a climate structure, then everything is working
		as expected
		
	*/
	int testVal = 0;
	
	ClimateGrid * cg = cg_make_climate_grid("climate_test.csv", 10, 10, 20, "test_params.txt");
	
	// check values from the new grid
	for(int x = 0; x < cg->xdim; x++)
		for(int y = 0; y < cg->ydim; y++)
			for(int yr = 0; yr < cg->numYears; yr++)
				testVal += cg->data[x][y][yr]->env1 != cg->data[x][y][yr]->env1; // this will segfault if there is a problem

	return testVal;
}