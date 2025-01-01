#ifndef SOLVER_H
#define SOLVER_H

#include "main.h"
#include "alg.h"
#include "cube_table.h"
#include "xcross4_table.h"
#include "xcross1_table.h"
#include "F2L_table.h"
#include "LL_table.h"
#include "move.h"

bool init_solver4();
void cleanup_solver4();
bool init_solver1();
void cleanup_solver1();

alg_s* solve_cube(cube18B_s cube, const F2L_table_s *f2l_table, const LL_table_s *ll_table);

LL_table_s* generate_last_layer_table(char *filename);
F2L_table_s* generate_f2l_table(char *filename);

#endif // SOLVER_H
