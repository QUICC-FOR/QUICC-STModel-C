#include <stdio.h>

#include "test.h"

#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_GREEN "\x1b[32m"

int main(void) {
	int failed = 0;
	
	failed += gc_test_all();
//	failed += io_test_all();
	failed += gr_test_all();
	
	test_func_print("Full test suite", failed);
}


void test_func_print(char funcName [], int numFailed) {
	if(numFailed)
		printf(ANSI_COLOR_CYAN "%s:" ANSI_COLOR_RED " failed %d tests" ANSI_COLOR_RESET "\n", funcName, numFailed);
	else
		printf(ANSI_COLOR_CYAN "%s:" ANSI_COLOR_GREEN " all tests passed" ANSI_COLOR_RESET "\n", funcName);
}

