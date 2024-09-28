#ifndef CUBE_H
#define CUBE_H

#include "main.h"
#include "move.h"
#include "solver_print.h"

int compare_cubes(cube_s a, cube_s b);

void apply_move(cube_s *cube, move_s move);
void apply_move_list(cube_s *cube, move_list_s *moves);

void print_face(uint64_t face);
void print_cube(cube_s cube);
void print_cube_colors(cube_s cube);

#endif // CUBE_H
