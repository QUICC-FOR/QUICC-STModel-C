
#include "grid.h"
#include "io.h"
#include "grid_cell.h"

#define TIME_STEP number;

int main(int argc, char ** argv){

    make_grid();
    parse_input();
    set_grid(data);

for (y, y<TIME_STEP, y++){

    for (i, i<size_t, i++){
        update_prevalence();  /*Prevalence and state is going to change at each time step*/
        get_trans_prob();
        select_new_state();
    }

}
    process_output();

    return 0;

}