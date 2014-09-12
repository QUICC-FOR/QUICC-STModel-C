#include <assert.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#include "../src/grid_cell.c"

int main() {
    /* DESC: creates a cell, assigns values to it, and tries to get them back out */ 
    
    // Init cell
    size_t histSize = 10;
    GridCell * cell = gc_make_cell(histSize,0,0);
    
    // Define first state
    *(cell->currentState) = DECIDUOUS;

    // Assertion of the state and the cell
    assert(cell->stateHistory[0] == DECIDUOUS);
    assert(cell->historySize == histSize);

    // Fill state history
    for(int i = 1; i < histSize; i++) {
        cell->currentState++;
        *(cell->currentState) = TRANSITIONAL;
    }

    // Assert value of the last time step
    assert(cell->stateHistory[histSize-1] == TRANSITIONAL);
    
    // Basic writing test in cell struct
    cell->climate.meanTemp = 0;
    cell->prevalence[0] = cell->transitionProbs[0] = 0.5;
    
    // Free memory
    gc_destroy_cell(cell);
}