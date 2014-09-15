#ifndef CLIMATE_H
#define CLIMATE_H

/*
	This library includes a very rudimentary CSV parser for accepting climate data as input
	It will break if you throw anything unexpected at it, so the input format must be correct
		Line endings MUST be UNIX-style (\n)
		There should be no quotes or other non-numeric characters in the numeric fields
		Delimiter must be a , or ;
		Decimal delimiter must be a .
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