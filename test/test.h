#ifndef TEST_H
#define TEST_H

#define TEST_ERROR_THRESHOLD 0.005
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_GREEN "\x1b[32m"

void test_func_print(char funcName [], int numFailed) {
	if(numFailed)
		printf(ANSI_COLOR_CYAN "%s:" ANSI_COLOR_RED " failed %d tests" ANSI_COLOR_RESET "\n", funcName, numFailed);
	else
		printf(ANSI_COLOR_CYAN "%s:" ANSI_COLOR_GREEN " all tests passed" ANSI_COLOR_RESET "\n", funcName);
}


#endif