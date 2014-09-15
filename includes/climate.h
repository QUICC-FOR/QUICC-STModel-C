#ifndef CLIMATE_H
#define CLIMATE_H

/*
	the expected format for the input data is a comma-separated file (, or ;)
	There should be one header line, which will be discarded
	the first column is the X-coordinate (longitude) of the cell
	the second column is the Y-coordinate (latitude)
	the third column is the time
		note that coordinates and time should be scaled, so that the minimum is 0, and the
		increment is 1. For time, the resolution is the same as the time step of the model
	subsequent columns are the data, with decimals separated by a .
	fourth: mean annual temperature

*/

typedef struct {
    double meanTemp;
    /*Task: list all variables need to be used*/
    // Fill later
} Climate;

typedef struct {
	Climate **** data;
	int xdim;
	int ydim;
	int numYears;
} ClimateGrid;

ClimateGrid * cg_make_climate_grid(const char * inputFile, int xdim, int ydim, int numYears);



#endif