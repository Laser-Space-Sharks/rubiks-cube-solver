#ifndef SOLVER_H
#define SOLVER_H

#include "main.h"
#include "move_list.h"
#include "cube_table.h"

cube_s masked_cube(const cube_s *cube, const cube_s *mask);
cube_s get_cross_edges(const cube_s *cube);


int stage_recursion(cube_s *cube, const cube_s *mask, const cube_s *goal, move_list_s *moves, uint8_t depth);
move_list_s* solve_stage(cube_s cube, cube_s mask);

int bidirectional_recursion(cube_s *cube, cube_table_s *our_ct, cube_table_s *other_ct, move_list_s *moves, uint8_t depth);
move_list_s* solve_cross(cube_s cube);

cube_table_s* generate_last_layer_table(char *filename);

#endif // SOLVER_H
