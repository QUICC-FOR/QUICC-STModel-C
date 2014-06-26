#include <stdio.h>
#include "../grid_cell.h"

int check_sums();
int check_boundary_conditions();
int check_overflow_condition();
int check_inputs();
int check_return_values();

int main(void) {
	int failedTests = 0;
	int numTests = 5;
	
	failedTests += check_sums();
	failedTests += check_boundary_conditions();
	failedTests += check_overflow_condition();
	failedTests += check_inputs();
	failedTests += check_return_values();
	
	printf("Failed %d of %d tests", failedTests, numTests);
	return failedTests;
	
}


int check_sums() {return 1;}
int check_boundary_conditions() {return 1;}
int check_overflow_condition() {return 1;}
int check_inputs() {return 1;}
int check_return_values() {return 1;}