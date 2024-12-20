#ifndef SOLVER_H
#define SOLVER_H

#include "main.h"
#include "alg.h"
#include "shift_cube.h"
#include "cube_table.h"
#include "xcross_table.h"
#include "F2L_table.h"
#include "LL_table.h"

bool init_solver();
void cleanup_solver();

int bidirectional_recursion(cube18B_xcross_s *cube, xcross_table_s *our_ct, xcross_table_s *other_ct, alg_s *moves, uint8_t depth);

alg_s* solve_cube(cube18B_s cube, const F2L_table_s *f2l_table, const LL_table_s *ll_table);

LL_table_s* generate_last_layer_table(char *filename);
F2L_table_s* generate_f2l_table(char *filename);

#endif // SOLVER_H
