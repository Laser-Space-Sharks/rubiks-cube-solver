#ifndef TESTS_H
#define TESTS_H

#include "shift_cube.h"
#include "cube18B.h"
#include "solver.h"
#include "translators.h"
#include "servoCoder.h"
#include "LL_stuff.h"

void test_translation(const shift_cube_s* shiftcube, const cube18B_s* cube18B);
void stress_test_shiftcube(size_t apply_alg_times, const alg_s* alg);
void stress_test_cube18B(size_t apply_alg_times, const alg_s* alg);
void stress_test_cube18B_xcross(size_t apply_alg_times, const alg_s* alg);
void stress_test(size_t apply_alg_times, const char* algstr);
void test_shiftcube_moves();
void test_cube18B_moves();
void test_cube_solve(const char** scrambles, int NUM_TESTS);
void test_simplifier_1case(char* algstr, char* simplifiedalgstr);
void test_simplifer();
void test_servoCoderC(const char** scrambles, size_t NUM_TESTS);
void test_solve_and_compile(const char** scrambles, size_t NUM_TESTS);
void test_LL_improvements();

#endif // TESTS_H
