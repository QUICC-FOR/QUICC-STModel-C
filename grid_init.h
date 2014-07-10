#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <gsl/gsl_rng.h>

void set_random_grid(Grid* grid);
void set_uniform_grid(Grid* grid);
void set_disturb_grid( Grid* grid, double thresDist);