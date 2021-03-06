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
		fourth: mean annual temperature (?)
		fifth: mean annual precipitation (?)

*/
#include <stdio.h>

#define MAX_LINE_LEN 4095
#define CL_NUM_TERMS 7


typedef struct {
    double env1;
    double env2;
} Climate;

typedef struct {
	double alphaB [CL_NUM_TERMS];
	double alphaT [CL_NUM_TERMS];
	double betaB [CL_NUM_TERMS];
	double betaT [CL_NUM_TERMS];
// 	double thetaB [CL_NUM_TERMS];
	double theta [CL_NUM_TERMS];
	double thetaT [CL_NUM_TERMS];
	double epsi [CL_NUM_TERMS];
} ClimatePars;

typedef struct {
	Climate **** data;
	int xdim;
	int ydim;
	int numYears;
	ClimatePars parameters;
	
} ClimateGrid;

ClimateGrid * cg_make_climate_grid(const char * inputFile, int xdim, int ydim, int numYears, const char * parfile);

/*
	cg_climate_from_grid: extract climate data from the climate grid cg
	year: the year at which to extract data
	x: the x-coordinate of the FINE-SCALE grid from which to extract data
	y: the y-coordinate of the FINE-SCALE grid from which to extract data
	xdim, ydim: the dimensions of the fine scale grid; will be used for finding the
		appropriate coordinates in the climate dimension
*/
Climate * cg_climate_from_grid(ClimateGrid * cg, int year, int x, int y, int xdim, int ydim);



/*
	initialize the parameter struct pars
	will use the input file parfile if available; if it is null or an empty string
	will initialize default values
*/
int cg_initialize_parameters(ClimatePars * pars, const char * parfile);


/*
	returns non-zero value if the climate object passed contains any null data
	returns zero otherwize
*/
int cl_climate_is_null(Climate * clim);

/*
	simple utility function for reading a single line (terminated with '\n') from an input
	file. The result is stored in the variable line; be sure line is large enough to contain
	the data; by default this is defined by the variable MAX_LINE_LEN, which specifies the
	maximum number of characters on a line.
*/
void readline(FILE * file, char * line);

#endif