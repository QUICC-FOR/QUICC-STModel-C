#include "climate.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define MAX_LINE_LEN 4095

static void cg_add_climate_record(ClimateGrid * cg, char * line);
static int cg_check_for_missing_climate_data(ClimateGrid * cg);
static void readline(FILE * file, char * line);

ClimateGrid * cg_make_climate_grid(const char * inputFile, int xdim, int ydim, int numYears)
{
	fprintf(stderr, "Reading climate data from file %s...\n", inputFile);
	ClimateGrid * newClimateGrid = malloc(sizeof(*newClimateGrid));
	assert(newClimateGrid);
	
	newClimateGrid->xdim = xdim;
	newClimateGrid->ydim = ydim;
	newClimateGrid->numYears = numYears;
	
	newClimateGrid->data = malloc(xdim * sizeof(*(newClimateGrid->data)));
	assert(newClimateGrid->data);
	for(int x = 0; x < xdim; x++) {
		newClimateGrid->data[x] = malloc(ydim * sizeof(*(newClimateGrid->data[x])));
		assert(newClimateGrid->data[x]);
		for(int y = 0; y < ydim; y++) {
			newClimateGrid->data[x][y] = malloc(numYears * sizeof(*(newClimateGrid->data[x][y])));
			assert(newClimateGrid->data[x][y]);
			//initialize the years to NULL; this way we can know if we are missing anyone
			for(int year = 0; year < numYears; year++) {
				newClimateGrid->data[x][y][year] = NULL;
			}
		}
	}
	
	FILE * iFile = fopen(inputFile, "r");
	if(!iFile) {
		fprintf(stderr, "Cannot open file %s\n", inputFile);
		exit(EXIT_FAILURE);
	}
	char line [MAX_LINE_LEN+1];
	readline(iFile, line);				// get rid of the header line
	readline(iFile, line);
	while(line[0]) {
		cg_add_climate_record(newClimateGrid, line);
		readline(iFile, line);
	}
	fclose(iFile);


	if(cg_check_for_missing_climate_data(newClimateGrid))
		exit(EXIT_FAILURE);

	return newClimateGrid;
}




static void readline(FILE * file, char * line) {
	char ch = getc(file);
	int len = 0;
	while (ch != '\n' && ch != EOF) {
		if(len >= MAX_LINE_LEN) {
			fprintf(stderr, "Buffer overflow; try shorter lines or increase MAX_LINE_LEN\n");
			exit(EXIT_FAILURE);
		}
		line[len++] = ch;
		ch = getc(file);
	}
	line[len] = '\0';
}




static void cg_add_climate_record(ClimateGrid * cg, char * line)
{
/*
	parses the variable into line into the expected climate format
	allocates memory for a new climate object
	then stores this in the cg->data array
*/
	Climate * newClimate = malloc(sizeof(*newClimate));
	assert(newClimate);

	char * cell = strtok(line, ",;");
	int longitude = strtol(cell, NULL, 10);
	cell = strtok(NULL, ",;");
	int latitude = strtol(cell, NULL, 10);
	cell = strtok(NULL, ",;");
	int year = strtol(cell, NULL, 10);

	// make sure we aren't writing over anything
	if(cg->data[longitude][latitude][year] != NULL) {
		fprintf(stderr, "Duplicate data for long=%d, lat=%d, year=%d\n", longitude, latitude, year);
		exit(EXIT_FAILURE);
	}

	// start on the data
	cell = strtok(NULL, ",;");
	newClimate->meanTemp = strtod(cell, NULL);
	cg->data[longitude][latitude][year] = newClimate;

}



static int cg_check_for_missing_climate_data(ClimateGrid * cg)
{
	int exitCode = 0;
	for(int x = 0; x < cg->xdim; x++) {
		for(int y = 0; y < cg->ydim; y++) {
			for(int year = 0; year < cg->numYears; year++) {
				if(cg->data[x][y][year] == NULL) {
					fprintf(stderr, "Missing climate data at x=%d, y=%d, year=%d\n", x, y, year);
					exitCode = 1;
				}
			}
		}
	}
	return exitCode;
}