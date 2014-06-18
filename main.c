
#include "grid.h"

#include "simulation.h"
#include "io.h"

int main(int argc, char ** argv){

    make_grid();
    parse_input();
    set_grid(data);

    run_simulation();
    process_output();

    return 0;

}