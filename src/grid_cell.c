#include <stdlib.h>
#include <assert.h>

#include "grid_cell.h"
#include "grid.h"

// prototypes for functions defining model parameters
static double beta_d(Climate climate);
static double beta_c(Climate climate);
static double theta_c(Climate climate);
static double theta_d(Climate climate);
static double phi_d(Climate climate);
static double phi_c(Climate climate);
static double phi_m(Climate climate);
static double epsi(Climate climate);

void gc_get_trans_prob(GridCell *cell) {
  /*
          calculate the transition probabilities given the current state
          values are automatically stored in cell->transitionProbs
  */

  switch (*(cell->currentState)) {

  case TRANSITIONAL:
    cell->transitionProbs[DECIDUOUS] = phi_d(cell->climate);
    cell->transitionProbs[CONIFEROUS] = phi_c(cell->climate);
    cell->transitionProbs[MIXED] = phi_m(cell->climate);
    cell->transitionProbs[TRANSITIONAL] =
        1 - (cell->transitionProbs[MIXED] + cell->transitionProbs[CONIFEROUS] +
             cell->transitionProbs[DECIDUOUS]);
    break;

  case MIXED:
    cell->transitionProbs[DECIDUOUS] = theta_d(cell->climate);
    cell->transitionProbs[CONIFEROUS] = theta_c(cell->climate);
    cell->transitionProbs[TRANSITIONAL] = epsi(cell->climate);
    cell->transitionProbs[MIXED] = 1 - (cell->transitionProbs[TRANSITIONAL] +
                                        cell->transitionProbs[CONIFEROUS] +
                                        cell->transitionProbs[DECIDUOUS]);
    break;

  case DECIDUOUS:
    cell->transitionProbs[CONIFEROUS] = 0;
    cell->transitionProbs[TRANSITIONAL] = epsi(cell->climate);
    cell->transitionProbs[MIXED] =
        beta_c(cell->climate) *
        (cell->prevalence[CONIFEROUS] + cell->prevalence[MIXED]);
    cell->transitionProbs[DECIDUOUS] =
        1 -
        (cell->transitionProbs[TRANSITIONAL] + cell->transitionProbs[MIXED]);
    break;

  case CONIFEROUS:
    cell->transitionProbs[DECIDUOUS] = 0;
    cell->transitionProbs[TRANSITIONAL] = epsi(cell->climate);
    cell->transitionProbs[MIXED] =
        beta_d(cell->climate) *
        (cell->prevalence[MIXED] + cell->prevalence[DECIDUOUS]);
    cell->transitionProbs[CONIFEROUS] =
        1 -
        (cell->transitionProbs[TRANSITIONAL] + cell->transitionProbs[MIXED]);
    break;

  default:
    break;
  }
}

GridCell *gc_make_cell(size_t numTimeSteps) {
  // allocates memory for the state history, initializes other values to 0
  // initializes the current state to the first record in stateHistory
  GridCell *cell = (GridCell *)malloc(sizeof(GridCell));
  cell->stateHistory = (State *)malloc(numTimeSteps * sizeof(State));
  assert(cell->stateHistory);
  cell->currentState = cell->stateHistory;
  cell->historySize = numTimeSteps;
  return cell;
}

void gc_select_new_state(GridCell *cell, gsl_rng *rng) {
  double rValue = gsl_rng_uniform(rng);
  double testVal = 0;
  State newState = 0;
  State GC_POSSIBLE_STATES[GC_NUM_STATES] = { MIXED, DECIDUOUS, CONIFEROUS,
                                              TRANSITIONAL };

  for (int i = 0; i < GC_NUM_STATES; i++) {
    State curState = GC_POSSIBLE_STATES[i];
    testVal += cell->transitionProbs[curState];
    if (rValue < testVal) {
      newState = curState;
      break;
    }
  }

  cell->currentState++;

  // check that we haven't wandered into invalid memory
  assert(cell->currentState <= (cell->stateHistory + (cell->historySize)));

  *(cell->currentState) = newState;
}

void gc_destroy_cell(GridCell *cell) {

  cell->currentState = NULL;
  free(cell->stateHistory);
  // refactor:
  // free(cell)
}

char st_state_to_char(State s) {

  switch (s) {
  case CONIFEROUS:
    return 'C';
    break;

  case TRANSITIONAL:
    return 'T';
    break;

  case DECIDUOUS:
    return 'D';
    break;

  case MIXED:
    return 'M';
    break;
  }

  return 0;

}

// TO IMPLEMENT
// FOR NOW, THESE RETURN A DUMMY VALUE; EVENTUALLY WILL RETURN THE RESULTS OF
// STATISTICAL MODELS
static double beta_d(Climate climate) {
  return 0.5;
}
static double beta_c(Climate climate) {
  return 0.5;
}
static double theta_c(Climate climate) {
  return 0.25;
}
static double theta_d(Climate climate) {
  return 0.25;
}
static double phi_d(Climate climate) {
  return 0.2;
}
static double phi_c(Climate climate) {
  return 0.2;
}
static double phi_m(Climate climate) {
  return 0.2;
}
static double epsi(Climate climate) {
  return 0.1;
}
