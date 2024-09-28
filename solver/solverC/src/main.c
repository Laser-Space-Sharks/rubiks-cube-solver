#include <unistd.h>

#include "main.h"
#include "cube.h"
#include "move.h"

int main(int argc, char *argv[]) {
    cube_s cube = SOLVED_SHIFTCUBE;

    printf("Initial Cube State:\n");
    print_cube_colors(cube);
    printf("\n");

    move_list_s moves;
    init_move_list(&moves, 10);

    printf("Algorithm to apply:\n");
    if (argc > 1) {
        move_list_from_move_str(&moves, argv[1]);
    } else {
        move_list_from_move_str(&moves, "U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2");
    }
    print_move_list(moves);
    apply_move_list(&cube, &moves);
    printf("\n");

    printf("Final Cube State:\n");
    print_cube_colors(cube);

    free_move_list(&moves);
    return 0;
}
