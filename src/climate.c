#include "climate.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>


/*
	TEMPORARY: here is some R code for generating fake climate data for testing
	
xm = 9
ym = 9
yrm = 19

xd = 0:xm
env2 = seq(700, 1600, length.out=length(xd))

yd = 0:ym
env1 = seq(7, -2, length.out=length(yd))

yr = 0:yrm

xx = rep(xd, length(yd))
xenv2 = rep(env2, length(yd))

yy = rep(yd, each=length(xd))
yenv1 = rep(env1, each=length(xd))

testClim = data.frame(
	x = rep(xx, length(yr)),
	y = rep(yy, length(yr)),
	year = rep(yr, each=length(xx)),
	env1 = rep(yenv1, length(yr)),
	env2 = rep(xenv2, length(yr))
)
write.table(testClim, paste(getwd(),"/climate_test.csv",sep=""), sep=',', row.names=FALSE)



###### what follows is a different set of climate data for use with Dom's parameters
xm = 9
ym = 19

xd = 0:xm
# precip in meters
env2 = seq(1, 1.3, length.out=length(xd))

yd = 0:ym
# temp in degrees
env1 = seq(6, 1, length.out=length(yd))

xx = rep(xd, length(yd))
xenv2 = rep(env2, length(yd))

yy = rep(yd, each=length(xd))
yenv1 = rep(env1, each=length(xd))

testClim = data.frame(
	x = xx,
	y = yy,
	year = rep(0, length(xx)),
	env1 = yenv1,
	env2 = xenv2)

write.table(testClim, "/Users/mtalluto/Documents/git_projects/C-STMFor/climate_test_dom.csv", sep=',', row.names=FALSE)

*/

static char CL_ERR_MSG [1024] = "";
static int cl_climateNullValue = -9999;

static inline int cg_add_climate_record(ClimateGrid * cg, char * line);
static inline int cg_check_for_missing_climate_data(ClimateGrid * cg);
static inline int parse_input_parameter(ClimatePars * p, char which, char state, int ind, char * datStr);
static inline int cg_parse_pars_from_file(ClimatePars * pars, const char * parfile);
static inline void cl_error();

Climate * cg_climate_from_grid(ClimateGrid * cg, int year, int x, int y, int xdim, int ydim)
{
	int cgX = x / (xdim/cg->xdim);
	int cgY = y / (ydim/cg->ydim);
	return cg->data[cgX][cgY][year];
}


ClimateGrid * cg_make_climate_grid(const char * inputFile, int xdim, int ydim, int numYears, const char * parfile)
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
	int err = 0;
	if(!iFile) {
		sprintf(CL_ERR_MSG, "Cannot open file %s\n", inputFile);
		err = 1;
	} else {
		char line [MAX_LINE_LEN+1];
		readline(iFile, line);				// get rid of the header line
		readline(iFile, line);
		while(line[0] && !err) {
			err = cg_add_climate_record(newClimateGrid, line);
			readline(iFile, line);
		}
		fclose(iFile);
	}

	if(err || cg_check_for_missing_climate_data(newClimateGrid) || cg_initialize_parameters(&(newClimateGrid->parameters), parfile))
		cl_error();

	return newClimateGrid;
}




void readline(FILE * file, char * line) {
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




int cg_initialize_parameters(ClimatePars * pars, const char * parfile)
{
	int returnState = 0;
	
	// set defaults
	pars->alphaB[0] = pars->alphaT[0] = -1.4;
	pars->betaB[0] = pars->betaT[0] = 0;
	pars->thetaB[0] = pars->thetaT[0] = -1.1;
	pars->epsi[0] = -2.2;	
	for(int i = 1; i < CL_NUM_TERMS; i++) {
		pars->alphaB[i] = pars->alphaT[i] = pars->betaB[i] = pars->betaT[i] = 0;
		pars->thetaB[i] = pars->thetaT[i] = pars->epsi[i] = 0;
	}
	
	// read parameters from a file
	if(parfile) 
		returnState = cg_parse_pars_from_file(pars, parfile);
	
	return returnState;
}


static inline int cg_parse_pars_from_file(ClimatePars * pars, const char * parfile)
{
	char * line = malloc(MAX_LINE_LEN+1 * sizeof(*line));
	int returnError = 0;
	FILE * iFile = fopen(parfile, "r");
	if(!iFile) {
		sprintf(CL_ERR_MSG, "Cannot open file %s", parfile);
		returnError = 1;
	} else {
		readline(iFile, line);
		while(line[0] && !returnError) {
			// this grabs the type (and optionally the state) of the parameter from the first
			// 1 to 2 characters of the line, moving the start of the line at the same time
			// so that it is aligned on the numeric data after reading; this is then passed
			// to the parsing function to actually store the data
			char * lineStart = line;
			char partype = *lineStart++;
			char parstate = '\0';
			if(partype != 'e')
				parstate = *lineStart++;

			char * err;
			int parindex = strtol(lineStart, &err, 0);
			if(err==lineStart) {
				sprintf(CL_ERR_MSG, "Missing index: %s", line);
				returnError = 1;
			}
			lineStart = err;
			if(parse_input_parameter(pars, partype, parstate, parindex, lineStart)){
				sprintf(CL_ERR_MSG, "Error reading parameter %c%c%d", partype, parstate, parindex);
				returnError = 1;
			}
			readline(iFile, line);
		}
		fclose(iFile);
	}
	free(line);
	return returnError;
}


static inline void cl_error()
{
	fprintf(stderr, "%s\n", CL_ERR_MSG);
	exit(EXIT_FAILURE);
}


int cl_climate_is_null(Climate *clim)
{
	return (clim->env1 == cl_climateNullValue || clim->env2 == cl_climateNullValue);
}



static inline int parse_input_parameter(ClimatePars * p, char which, char state, int ind, char * datStr)
{
	/*
		reads a parameter: the character in which determines the par to read
			a: alpha
			b: beta
			t: theta
			e: epsilon
		state determines the state to which the parameter applies
			b: boreal
			t: temperate
		ind tells us which parameter to use
		datStr is the string containing the data; leading whitespace is ignored
		so which='a', state='b', ind=5, datStr="  -1.45" would set parameter
		  ab5 = -1.45
	*/
	
	int errVal = 0;
	char * err;
	double data = strtod(datStr, &err);
	if(err == datStr || ind < 0 || ind >= CL_NUM_TERMS)
		which = '\0'; // this will trigger an error in the switch below & avoid processing invalid data
	
	fprintf(stderr, "%c%c%d = %f\n", which, state, ind, data);
	switch(which) {
	case 'a':
		switch(state) {
		case 'b':
			p->alphaB[ind] = data;
			break;
		case 't':
			p->alphaT[ind] = data;
			break;
		default:
			errVal = 1;
		}
		break;
	
	case 'b':
		switch(state) {
		case 'b':
			p->betaB[ind] = data;
			break;
		case 't':
			p->betaT[ind] = data;
			break;
		default:
			errVal = 1;
		}
		break;
	
	case 't':
		switch(state) {
		case 'b':
			p->thetaB[ind] = data;
			break;
		case 't':
			p->thetaT[ind] = data;
			break;
		default:
			errVal = 1;
		}
		break;
	
	case 'e':
		p->epsi[ind] = data;
		break;
	
	default:
		errVal = 1;
	}
	return errVal;
}

static inline int cg_add_climate_record(ClimateGrid * cg, char * line)
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
		sprintf(CL_ERR_MSG, "Duplicate data for long=%d, lat=%d, year=%d, env1=%f, env2=%f", longitude, latitude, year,cg->data[longitude][latitude][year]->env1, cg->data[longitude][latitude][year]->env2);
		return 1;
	}

	// start on the data
	int errorVal = 0;
	char * val1 = strtok(NULL, ",;");
	char * val2 = strtok(NULL, ",;");
	if(val1 && val2) {
		char *err1, *err2;
		newClimate->env1 = strtod(val1, &err1);
		newClimate->env2 = strtod(val2, &err2);
		errorVal = errorVal || err1 == val1 || err2 == val2;
	} else
		errorVal = 1;

	if(errorVal) {
		sprintf(CL_ERR_MSG, "Error converting climate data value for long=%d, lat=%d, year=%d", longitude, latitude, year);
		free(newClimate);
	}
	else
		cg->data[longitude][latitude][year] = newClimate;
	return errorVal;
}



static inline int cg_check_for_missing_climate_data(ClimateGrid * cg)
{
	for(int x = 0; x < cg->xdim; x++) {
		for(int y = 0; y < cg->ydim; y++) {
			for(int year = 0; year < cg->numYears; year++) {
				if(cg->data[x][y][year] == NULL) {
					sprintf(CL_ERR_MSG, "Missing climate data at x=%d, y=%d, year=%d", x, y, year);
					return 1;
				}
			}
		}
	}
	return 0;
}