#ifndef SOLVER_H
#define SOLVER_H

#include "main.h"
#include "alg.h"
#include "shift_cube.h"
#include "cube_table.h"
#include "cube_alg_table.h"

bool init_solver();
void cleanup_solver();

int stage_recursion(shift_cube_s *cube, const shift_cube_s *mask, const shift_cube_s *goal, alg_s *moves, uint8_t depth);
alg_s* solve_stage(shift_cube_s cube, shift_cube_s mask);

int bidirectional_recursion(shift_cube_s *cube, cube_alg_table_s *our_ct, 
                            cube_alg_table_s *other_ct, alg_s *moves, uint8_t depth);
alg_s* bidirectional_search(const shift_cube_s *start, const shift_cube_s *goal, uint8_t max_depth);

alg_s* solve_cross(shift_cube_s cube);

alg_s* solve_cube(shift_cube_s cube, const cube_table_s *f2l_table, const cube_alg_table_s *ll_table);
alg_s* solve_f2l(shift_cube_s cube);

cube_table_s* gen_f2l_table();
cube_alg_table_s* gen_last_layer_table();

#endif // SOLVER_H
