#include "main.h"
#include "cube.h"
#include "alg.h"
#include "solver.h"

int main(int argc, char *argv[]) {
    init_move_bitrolls();

    cube_s cube = SOLVED_SHIFTCUBE;

    cube_table_s *f2l_table = generate_f2l_table("../../ALGORITHMS/FULL_F2L_ALGORITHMS.txt");
    cube_table_s *last_layer_table = generate_last_layer_table("../../ALGORITHMS/FULL_1LLL_ALGORITHMS.txt");

    init_solver();

    alg_s *alg = NULL;
    #define NUM_TESTS 9
    char *scrambles[] = {
        "F D' R2 D' L' F L B' U R D' R F' U2 F D R U' F' D2 L U' R2 B' U2",
        "L' B R2 F2 L' B L' D' F' L' D2 R' B' R F R' F R F U L B L U' R'",
        "D F L U B' U' L2 B' L' B' U' R' D F' D' L2 D F L U L' D2 L U L'",
        "B2 D R' F' R2 B' D2 L2 D B2 D L' F D2 L2 D L' F' R2 U L' D' F U B'",
        "R' D F L' D' R' D F2 R' F' R' B' R F2 R B' U F' L' D B2 L' D L' F",
        "L' B D F' L' B D2 B L' B' D L' U B L D R' B2 R D2 R U L D' B",
        "D B' L' D F' R' D L F2 U F D' L F' L' F' D' L U' B D R B' U2 F",
        "L2 D R2 F D R2 U2 R' F' R' F' L F D R B' U R' U F' D B' R' B R'",
        "F2 U L' U R' U L U B' L F D' F' U' R' D F2 R B' L D2 B' L' F' L'"
    };

    if (argc > 1) {
        alg = alg_from_alg_str(argv[1]);
    } else {
        printf("Performing tests\n");
        double sum = 0;

        for (uint8_t test = 0; test < NUM_TESTS; test++) {
            cube = SOLVED_SHIFTCUBE;
            alg = alg_from_alg_str(scrambles[test]);
            printf("Testing scramble: %s\n", scrambles[test]);
            apply_alg(&cube, alg);
            alg_s *solve = solve_cube(cube, f2l_table, last_layer_table);
            apply_alg(&cube, solve);
            if (!compare_cubes(&cube, &SOLVED_SHIFTCUBE)) {
                printf("It didn't solve it, this is bad...\n");
            }
            cube = SOLVED_SHIFTCUBE;
            printf("Solution (%zu moves): ", solve->length);
            print_alg(solve);
            sum += solve->length;
            alg_free(solve);
            alg_free(alg);
        }
        printf("Average solve length: %f\n", sum / NUM_TESTS);
    }

    cube_table_free(f2l_table);
    cube_table_free(last_layer_table);
    cleanup_solver();

    return 0;
}
