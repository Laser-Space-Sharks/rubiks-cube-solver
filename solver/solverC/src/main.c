#include "main.h"
#include "cube.h"
#include "move_list.h"
#include "solver.h"

int main(int argc, char *argv[]) {
    cube_s cube = SOLVED_SHIFTCUBE;

    move_list_s *moves = NULL;
    if (argc > 1) {
        moves = move_list_from_move_str(argv[1]);
    } else {
        moves = move_list_from_move_str("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2");
    }

    print_move_list(moves);
    printf("\n");
    apply_move_list(&cube, moves);

    printf("Initial Cube State:\n");
    print_cube_map_colors(cube);
    printf("\n");

    move_list_s *cross_solve = solve_stage(cube, cross_mask);
    printf("Solution!:\n");
    print_move_list(cross_solve);
    apply_move_list(&cube, cross_solve);
    printf("New cube state:\n");
    print_cube_map_colors(cube);

    printf("Now for F2L 1st pair...\n");
    move_list_s *f2l_1solve = solve_stage(cube, f2l_1mask);
    printf("Solution!:\n");
    print_move_list(f2l_1solve);
    apply_move_list(&cube, f2l_1solve);

    printf("Now for F2L 2nd pair...\n");
    move_list_s *f2l_2solve = solve_stage(cube, f2l_2mask);
    printf("Solution!:\n");
    print_move_list(f2l_2solve);
    apply_move_list(&cube, f2l_2solve);

    printf("Now for F2L 3rd pair...\n");
    move_list_s *f2l_3solve = solve_stage(cube, f2l_3mask);
    printf("Solution!:\n");
    print_move_list(f2l_3solve);
    apply_move_list(&cube, f2l_3solve);

    printf("Now for F2L Final pair...\n");
    move_list_s *f2l_4solve = solve_stage(cube, f2l_4mask);
    printf("Solution!:\n");
    print_move_list(f2l_4solve);
    apply_move_list(&cube, f2l_4solve);
    printf("New cube state:\n");
    print_cube_map_colors(cube);


    printf("Final Cube State:\n");
    print_cube_map_colors(cube);
    print_cube_line_colors(cube);

    move_list_free(moves);
    move_list_free(cross_solve);
//    move_list_free(f2l_1solve);
//    move_list_free(f2l_2solve);
//    move_list_free(f2l_3solve);
//    move_list_free(f2l_4solve);

    return 0;
}
