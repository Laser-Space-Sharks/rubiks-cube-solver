#ifndef CUBE_H
#define CUBE_H

#include <stdbool.h>

#include "main.h"
#include "solver_print.h"

cube_s* cube_copy(const cube_s *cube);

bool compare_cubes(const cube_s *a, const cube_s *b);

void apply_move(cube_s *cube, move_s move);
void apply_move_list(cube_s *cube, const move_list_s *moves);

void print_face(uint32_t face);

void print_cube_map(cube_s cube);
void print_cube_line(cube_s cube);
void print_cube_map_colors(cube_s cube);
void print_cube_line_colors(cube_s cube);

#endif // CUBE_H
