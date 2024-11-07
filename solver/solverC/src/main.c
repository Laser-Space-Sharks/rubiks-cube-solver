#include "main.h"
#include "cube.h"
#include "move_list.h"
#include "solver.h"

#include "lookup_tables.h"

int main(int argc, char *argv[]) {
    cube_s cube = SOLVED_SHIFTCUBE;

    move_list_s *moves = NULL;
    if (argc > 1) {
        moves = move_list_from_move_str(argv[1]);
    } else {
        moves = move_list_from_move_str("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2");
    }


    printf("Applying scramble to initial cube state: ");
    print_move_list(moves);
    printf("\n");
    apply_move_list(&cube, moves);

    printf("Scrambled cube state:\n");
    print_cube_map_colors(cube);
    printf("\n");

    printf("Solving cross...\n");
    print_cube_line_colors(cube);
    printf("=> ");
    move_list_s *cross_solve = solve_cross(cube);
    apply_move_list(&cube, cross_solve);
    print_cube_line_colors(cube);

    printf("\nSolving F2L 1st pair...\n");
    print_cube_line_colors(cube);
    printf("=> ");
    move_list_s *f2l_1solve = solve_stage(cube, f2l_1mask);
    apply_move_list(&cube, f2l_1solve);
    print_cube_line_colors(cube);

    printf("\nSolving F2L 2nd pair...\n");
    print_cube_line_colors(cube);
    printf("=> ");
    move_list_s *f2l_2solve = solve_stage(cube, f2l_2mask);
    apply_move_list(&cube, f2l_2solve);
    print_cube_line_colors(cube);

    printf("\nSolving F2L 3rd pair...\n");
    print_cube_line_colors(cube);
    printf("=> ");
    move_list_s *f2l_3solve = solve_stage(cube, f2l_3mask);
    apply_move_list(&cube, f2l_3solve);
    print_cube_line_colors(cube);

    printf("\nSolving F2L Final pair...\n");
    print_cube_line_colors(cube);
    printf("=> ");
    move_list_s *f2l_4solve = solve_stage(cube, f2l_4mask);
    apply_move_list(&cube, f2l_4solve);
    print_cube_line_colors(cube);

    cube_table_s *last_layer_table = generate_last_layer_table("../../ALGORITHMS/FULL_1LLL_ALGORITHMS.txt");
    if (last_layer_table == NULL) {
        printf("Last layer table failed to generate\n");
        return 1;
    }

    move_list_s *last_layer_solve = NULL;
    if (!compare_cubes(&cube, &SOLVED_SHIFTCUBE)) {
        printf("\nNow for 1LLL...\n");
        print_cube_line_colors(cube);
        printf("=> ");
        last_layer_solve = move_list_copy(cube_table_lookup(last_layer_table, &cube));

        apply_move_list(&cube, last_layer_solve);
        print_cube_line_colors(cube);
    }

    move_list_s *full_solve = move_list_concat(
        move_list_concat(
            move_list_concat(
                move_list_concat(cross_solve, f2l_1solve),
                move_list_concat(f2l_2solve, f2l_3solve)
            ), f2l_4solve),
        last_layer_solve
    );

    printf("\nSolved cube!\n");

    printf("\nFinal Cube State:\n");
    print_cube_map_colors(cube);
    printf("\n");

    printf("Full solve (%zu moves): ", full_solve->length);
    print_move_list(full_solve);

    move_list_free(moves);
    move_list_free(cross_solve);
    move_list_free(f2l_1solve);
    move_list_free(f2l_2solve);
    move_list_free(f2l_3solve);
    move_list_free(f2l_4solve);
    move_list_free(last_layer_solve);
    move_list_free(full_solve);
    cube_table_free(last_layer_table);

    return 0;
}
