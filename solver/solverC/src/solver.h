#ifndef SOLVER_H
#define SOLVER_H

#include "main.h"
#include "move_list.h"
#include "cube_table.h"

bool init_solver();
void cleanup_solver();

int stage_recursion(cube_s *cube, const cube_s *mask, const cube_s *goal, move_list_s *moves, uint8_t depth);
move_list_s* solve_stage(cube_s cube, cube_s mask);

int bidirectional_recursion(cube_s *cube, cube_table_s *our_ct, cube_table_s *other_ct, move_list_s *moves, uint8_t depth);
move_list_s* bidirectional_search(const cube_s *start, const cube_s *goal, uint8_t max_depth);

move_list_s* solve_cross(cube_s cube);

move_list_s* solve_cube(cube_s cube, const cube_table_s *f2l_table, const cube_table_s *ll_table);
move_list_s* solve_f2l(cube_s cube);

cube_table_s* generate_last_layer_table(char *filename);
cube_table_s* generate_f2l_table(char *filename);

#endif // SOLVER_H
