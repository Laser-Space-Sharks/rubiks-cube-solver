#include "main.h"
#include "alg.h"
#include "shift_cube.h"
#include "cube18B.h"
#include "translators.h"
#include "solver.h"
#include "servoCoder.h"

#include <time.h>

static void test_translation(const shift_cube_s* shiftcube, const cube18B_s* cube18B)  {
    cube18B_s translated_cube18B = cube18B_from_shiftCube(shiftcube);
    if (!compare_cube18Bs(&translated_cube18B, cube18B)) {
        printf("cube18B_from_shiftCube(shiftcube) and cube18B don't match!\n");
        printf("shiftcube: \n");
        print_cube_map_colors(*shiftcube);
        printf("shiftcube translated to cube18B: \n");
        print_cube18B(&translated_cube18B);
        printf("cube18B: \n");
        print_cube18B(cube18B);
        printf("solved_cube18B: \n");
        print_cube18B(&SOLVED_CUBE18B);
        printf("\n");
    }

    shift_cube_s translated_shiftcube = shiftCube_from_cube18B(cube18B);
    cube18B_s translated_translated_cube18B = cube18B_from_shiftCube(&translated_shiftcube);
    if (!compare_cube18Bs(&translated_translated_cube18B, cube18B)) {
        printf("ohno, the shiftcube_from_cube18B isn't working\n");
        print_cube_map_colors(*shiftcube);
        print_cube18B(cube18B);
        printf("\n");
        print_cube_map_colors(translated_shiftcube);
        print_cube18B(&translated_translated_cube18B);
        printf("\n");
    }
}
static void stress_test_shiftcube(size_t apply_alg_times, const alg_s* alg) {
    shift_cube_s shiftcube = SOLVED_SHIFTCUBE;
    printf("Stress-testing shiftcube with %zu moves...\n", apply_alg_times*(alg->length));
    clock_t start_shiftcube = clock();
    for (int i = 0; i < apply_alg_times; i++) {
        apply_alg(&shiftcube, alg);
    }
    clock_t end_shiftcube = clock();
    printf("Shiftcube time: %fs\n", (double)(end_shiftcube - start_shiftcube)/CLOCKS_PER_SEC);
}
static void stress_test_cube18B(size_t apply_alg_times, const alg_s* alg) {
    cube18B_s cube18B = SOLVED_CUBE18B;
    printf("Stress-testing cube18B with %zu moves...\n", apply_alg_times*(alg->length));

    clock_t start_cube18b = clock();
    for (int i = 0; i < apply_alg_times; i++) {
        cube18B_apply_alg(&cube18B, alg);
    } clock_t end_cube18b = clock();

    printf("Cube18B time: %fs\n", (double)(end_cube18b - start_cube18b)/CLOCKS_PER_SEC);
}
static void stress_test_cube18B_xcross(size_t apply_alg_times, const alg_s* alg) {
    cube18B_xcross_s xcross = SOLVED_CUBE18B_XCROSS;
    printf("Stress-testing cube18B_xcross with %zu moves...\n", apply_alg_times*(alg->length));

    clock_t start_cube18b = clock();
    for (int i = 0; i < apply_alg_times; i++) {
        cube18B_xcross_apply_alg(&xcross, alg);
    } clock_t end_cube18b = clock();

    printf("Cube18B_xcross time: %fs\n", (double)(end_cube18b - start_cube18b)/CLOCKS_PER_SEC);
}
static void stress_test(size_t apply_alg_times, const char* algstr) {
    alg_s* alg = alg_from_alg_str(algstr);
    printf("alg to stress test %d times: \n", apply_alg_times);
    print_alg(alg);
    stress_test_shiftcube(apply_alg_times, alg);
    stress_test_cube18B(apply_alg_times, alg);
    stress_test_cube18B_xcross(apply_alg_times, alg);
    printf("Finished stress-testing!\n");
    printf("\n");
    alg_free(alg);
}
static void test_shiftcube_moves() {
    shift_cube_s cube = SOLVED_SHIFTCUBE;
    print_cube_map_colors(cube); 
    printf("U moves:\n");
    apply_move(&cube, MOVE_U); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_U3); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_U2); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_U2); print_cube_map_colors(cube); 
    printf("R moves:\n");
    apply_move(&cube, MOVE_R); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_R3); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_R2); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_R2); print_cube_map_colors(cube); 
    printf("F moves:\n");
    apply_move(&cube, MOVE_F); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_F3); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_F2); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_F2); print_cube_map_colors(cube); 
    printf("L moves:\n");
    apply_move(&cube, MOVE_L); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_L3); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_L2); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_L2); print_cube_map_colors(cube); 
    printf("B moves:\n");
    apply_move(&cube, MOVE_B); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_B3); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_B2); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_B2); print_cube_map_colors(cube); 
    printf("D moves:\n");
    apply_move(&cube, MOVE_D); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_D3); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_D2); print_cube_map_colors(cube); 
    apply_move(&cube, MOVE_D2); print_cube_map_colors(cube); 
    printf("\n");
}
static void test_cube18B_moves() {
    shift_cube_s cube = SOLVED_SHIFTCUBE;
    cube18B_s cube18B = SOLVED_CUBE18B;
    printf("U moves:\n");
    apply_move(&cube, MOVE_U);  cube18B_apply_move(&cube18B, MOVE_U); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_U3); cube18B_apply_move(&cube18B, MOVE_U3); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_U2); cube18B_apply_move(&cube18B, MOVE_U2); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_U2); cube18B_apply_move(&cube18B, MOVE_U2); test_translation(&cube, &cube18B);
    printf("R moves:\n");
    apply_move(&cube, MOVE_R);  cube18B_apply_move(&cube18B, MOVE_R); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_R3); cube18B_apply_move(&cube18B, MOVE_R3); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_R2); cube18B_apply_move(&cube18B, MOVE_R2); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_R2); cube18B_apply_move(&cube18B, MOVE_R2); test_translation(&cube, &cube18B);
    printf("F moves:\n");
    apply_move(&cube, MOVE_F);  cube18B_apply_move(&cube18B, MOVE_F); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_F3); cube18B_apply_move(&cube18B, MOVE_F3); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_F2); cube18B_apply_move(&cube18B, MOVE_F2); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_F2); cube18B_apply_move(&cube18B, MOVE_F2); test_translation(&cube, &cube18B);
    printf("L moves:\n");
    apply_move(&cube, MOVE_L);  cube18B_apply_move(&cube18B, MOVE_L); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_L3); cube18B_apply_move(&cube18B, MOVE_L3); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_L2); cube18B_apply_move(&cube18B, MOVE_L2); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_L2); cube18B_apply_move(&cube18B, MOVE_L2); test_translation(&cube, &cube18B);
    printf("B moves:\n");
    apply_move(&cube, MOVE_B);  cube18B_apply_move(&cube18B, MOVE_B); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_B3); cube18B_apply_move(&cube18B, MOVE_B3); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_B2); cube18B_apply_move(&cube18B, MOVE_B2); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_B2); cube18B_apply_move(&cube18B, MOVE_B2); test_translation(&cube, &cube18B);
    printf("D moves:\n");
    apply_move(&cube, MOVE_D);  cube18B_apply_move(&cube18B, MOVE_D); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_D3); cube18B_apply_move(&cube18B, MOVE_D3); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_D2); cube18B_apply_move(&cube18B, MOVE_D2); test_translation(&cube, &cube18B);
    apply_move(&cube, MOVE_D2); cube18B_apply_move(&cube18B, MOVE_D2); test_translation(&cube, &cube18B);
    printf("\n");
}
static void test_cube_solve(const char** scrambles, int NUM_TESTS) {
    init_solver();

    cube_table_s *f2l_table = generate_f2l_table("../../ALGORITHMS/FULL_F2L_ALGORITHMS.txt");
    cube_table_s *last_layer_table = generate_last_layer_table("../../ALGORITHMS/FULL_1LLL_ALGORITHMS.txt");

    alg_s *alg = NULL;
    shift_cube_s cube = SOLVED_SHIFTCUBE;

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
    printf("\n");

    cube_table_free(f2l_table);
    cube_table_free(last_layer_table);

    cleanup_solver();
}

static void test_simplifier_1case(char* algstr, char* simplifiedalgstr) {
    alg_s* alg = alg_from_alg_str(algstr);
    alg_simplify(alg);
    alg_s* simplified = alg_from_alg_str(simplifiedalgstr);
    if (alg->length != simplified->length) {
        printf("ALGS DON'T MATCH:\n");
        printf("Length of alg: %d\n", alg->length);
        print_alg(alg);
        printf("Length of simplified: %d\n", simplified->length);
        print_alg(simplified);
    } else {
        shift_cube_s cube1 = SOLVED_SHIFTCUBE, cube2 = SOLVED_SHIFTCUBE;
        apply_alg(&cube1, alg);
        apply_alg(&cube2, simplified);
        if (!compare_cubes(&cube1, &cube2)) {
            printf("ALGS DON'T MATCH:\n");
            print_alg(alg);
            print_alg(simplified);
        }
    }
    alg_free(alg);
    alg_free(simplified);
}

static void test_simplifer() {
    // can cancel out moves to result in a null alg
    test_simplifier_1case("U U'", "");                                          
    // can utilize the F B F -> F2 B simplification rule multiple times
    test_simplifier_1case("F U R3 L R2 L3 D", "F U R D");                       
    // heftier case with lots of simplification to really make sure these simplifications can happen repeatedly without inherent limit
    test_simplifier_1case("R L' R2 L3 U L L2 L3 D' U3 D2", "R3 L2 U L2 D U3");  
    // can recognize irreducibility
    test_simplifier_1case("R3 L2 U L2 D U3", "R3 L2 U L2 D U3");
}
static void test_servoCoderC(const char** scrambles, size_t NUM_TESTS) {
    const inter_move_table_s* INTER_MOVE_TABLE = inter_move_table_create();
    alg_s* alg = NULL;
    for (size_t i = 0; i < NUM_TESTS; i++) {
        alg = alg_from_alg_str(scrambles[i]);
        RobotSolution robosolution = servoCode_compiler_Ofastest(alg, INTER_MOVE_TABLE);
        //for (size_t i = 0; i < robosolution.size; i++) {
        //    print_RobotState(robosolution.solution[i]);
        //    printf("\n");
        //}
        alg_free(alg);
        alg = NULL;
        free(robosolution.solution);
    }
    inter_move_table_free(INTER_MOVE_TABLE);
}
static void test_solve_and_compile(const char** scrambles, size_t NUM_TESTS) {
    init_solver();
    const inter_move_table_s* INTER_MOVE_TABLE = inter_move_table_create();

    cube_table_s *f2l_table = generate_f2l_table("../../ALGORITHMS/FULL_F2L_ALGORITHMS.txt");
    cube_table_s *last_layer_table = generate_last_layer_table("../../ALGORITHMS/FULL_1LLL_ALGORITHMS.txt");

    alg_s *alg = NULL;
    shift_cube_s cube = SOLVED_SHIFTCUBE;

    printf("Performing tests\n");
    double sum_algLengths = 0;
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
        RobotSolution robosolution = servoCode_compiler_Ofastest(solve, INTER_MOVE_TABLE);
        sum_algLengths += solve->length;
        alg_free(solve);
        alg_free(alg);
        free(robosolution.solution);
    }
    printf("Average solve length: %f\n", sum_algLengths / NUM_TESTS);
    printf("\n");

    cube_table_free(f2l_table);
    cube_table_free(last_layer_table);

    cleanup_solver();
    inter_move_table_free(INTER_MOVE_TABLE);
}

int main(int argc, char *argv[]) {
    shift_cube_s cube = SOLVED_SHIFTCUBE;

    #define NUM_TESTS 9
    const char* scrambles[NUM_TESTS] = {
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
        alg_s* alg = alg_from_alg_str(argv[1]);
        alg_free(alg);
    } else {
        //test_cube_solve(scrambles, NUM_TESTS);

        //test_shiftcube_moves();
        //test_cube18B_moves();

        //stress_test(40000, scrambles[0]);

        //test_simplifer();

        //test_servoCoderC(scrambles, NUM_TESTS);

        test_solve_and_compile(scrambles, NUM_TESTS);
    }

    return 0;
}