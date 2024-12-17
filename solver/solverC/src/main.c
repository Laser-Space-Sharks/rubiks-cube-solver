#include "main.h"
#include "alg.h"
#include "shift_cube.h"
#include "cube18B.h"
#include "translators.h"
#include "solver.h"

#include <time.h>

int main(int argc, char *argv[]) {
    shift_cube_s cube = SOLVED_SHIFTCUBE;

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


        alg = alg_from_alg_str("F D' R2 D' L' F L B' U R D' R F' U2 F D R U' F' D2 L U' R2 B' U2");
        //init_all_tables_in_cube18Bc();
//        cubieTable_s cubieTable = alg_to_cubieTable(alg);
        shift_cube_s shiftcube = SOLVED_SHIFTCUBE;
        cube18B_s cube18B = SOLVED_CUBE18B;
//        cube18B_xcross_s xcross = SOLVED_CUBE18B_XCROSS;
//        print_cube_line_colors(shiftcube);
//        print_cube18B(&cube18B);
        //printf("FACE_NULL is %d\n", FACE_NULL);

        int apply_alg_times = 1000000;
        printf("alg to stress test %d times: %s\n", apply_alg_times, "F D' R2 D' L' F L B' U R D' R F' U2 F D R U' F' D2 L U' R2 B' U2");
        printf("Stress-testing cube18B with %zu moves...\n", apply_alg_times*(alg->length));
        clock_t start_cube18b = clock();
        for (int i = 0; i < apply_alg_times; i++) {
            cube18B_apply_alg(&cube18B, alg);
//            cube18B_xcross_apply_alg(&xcross, alg);
//            apply_cubieTable_to_cube(&cube18B, &cubieTable);
            //apply_alg(&shiftcube, alg);
        }
        clock_t end_cube18b = clock();
        printf("Stress-testing shiftcube with %zu moves...\n", apply_alg_times*(alg->length));
        clock_t start_shiftcube = clock();
        for (int i = 0; i < apply_alg_times; i++) {
            //cube18B_apply_alg(&cube18B, alg);
            apply_alg(&shiftcube, alg);
        }
        clock_t end_shiftcube = clock();
        printf("Finished stress-testing!\n");
        cube18B_s translated_cube18B = cube18B_from_shiftCube(&shiftcube);
        if (!compare_cube18Bs(&translated_cube18B, &cube18B)) {
            printf("cube18B_from_shiftCube(shiftcube) and cube18B don't match!\n");
            printf("shiftcube: \n");
            print_cube_map_colors(shiftcube);
            printf("shiftcube translated to cube18B: \n");
            print_cube18B(&translated_cube18B);
            printf("cube18B: \n");
            print_cube18B(&cube18B);
            printf("solved_cube18B: \n");
            print_cube18B(&SOLVED_CUBE18B);
        }

        shift_cube_s translated_shiftcube = shiftCube_from_cube18B(&cube18B);
        cube18B_s translated_translated_cube18B = cube18B_from_shiftCube(&translated_shiftcube);
        if (!compare_cube18Bs(&translated_translated_cube18B, &cube18B)) {
            printf("ohno, the shiftcube_from_cube18B isn't working\n");
            print_cube_map_colors(shiftcube);
            print_cube18B(&cube18B);
            printf("\n");
            print_cube_map_colors(translated_shiftcube);
            print_cube18B(&translated_translated_cube18B);
        }
        printf("Cube18B time: %fs\nShiftcube time: %fs\n", (double)(end_cube18b - start_cube18b)/CLOCKS_PER_SEC, (double)(end_shiftcube - start_shiftcube)/CLOCKS_PER_SEC);
        alg_free(alg);
        //print_cubieDefinitions();
        //print_cubieDefinition_to_cubie();
        //print_colorSequence_to_solvedCubieInd();
        //print_cubie_to_orderedPositions();
        //print_cubieAfterMove();
        //print_colorsAtEdgePosInd_to_cubieAndSolvedCubie();
        //print_colorsAtCornerPosInd_to_cubieAndSolvedCubie();
        //print_cubieAndSolvedCubieInd_to_colorsAtPosInd();
    }

//    move_s move_f = {
//        FACE_F,
//        1
//    };
//    move_s move_fprime = {
//        FACE_F,
//        3
//    };
//    move_s move_f2 = {
//        FACE_F,
//        2
//    };

//
//    clock_t new_move_start = clock();
//    apply_move(&cube, MOVE_D);
//    print_cube_map_colors(cube);
//    apply_move(&cube, MOVE_F3);
//    print_cube_map_colors(cube);
//    apply_move(&cube, MOVE_U2);
//    print_cube_map_colors(cube);
//    apply_move(&cube, MOVE_R2);
//    print_cube_map_colors(cube);
//
//    clock_t new_move_end = clock();
//    print_cube_map_colors(cube);
//
//    clock_t old_move_start = clock();
//    for (int i = 0; i < 100000000; i++) {
//        old_apply_move(&cube, move_f);
//        old_apply_move(&cube, move_fprime);
//        old_apply_move(&cube, move_f2);
//        old_apply_move(&cube, move_f2);
//    }
//
//    clock_t old_move_end = clock();
//
//    printf("Old Shiftcube move time: %fs\t New Shiftcube move time: %fs\n",
//           (double)(old_move_end - old_move_start)/CLOCKS_PER_SEC,
//           (double)(new_move_end - new_move_start)/CLOCKS_PER_SEC);



    //cube_table_free(f2l_table);
    //cube_table_free(last_layer_table);
    cleanup_solver();

    return 0;
}
