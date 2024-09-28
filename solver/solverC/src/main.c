#include <bits/time.h>
#include <time.h>
#include <unistd.h>

#include "main.h"
#include "cube.h"
#include "solver.h"
#include "move.h"

int main(int argc, char *argv[]) {
    cube_s cube = SOLVED_SHIFTCUBE;

    printf("Initial Cube State:\n");
    print_cube_colors(cube);
    printf("\n");

    move_list_s moves;
    init_move_list(&moves, 10);

    printf("Algorithm to apply:\n");
    move_list_from_move_str(&moves, "L2 R2 U2 D2 F2 B2");
    print_move_list(moves);
    apply_move_list(&cube, &moves);
    printf("\n");

    printf("Final Cube State:\n");
    print_cube_colors(cube);

    free_move_list(&moves);
    return 0;
}
