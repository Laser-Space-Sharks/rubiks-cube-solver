#ifndef CUBE_H
#define CUBE_H

#include <stdbool.h>

#include "main.h"
#include "solver_print.h"

cube_s* cube_copy(const cube_s *cube);
cube_s ored_cube(const cube_s *a, const cube_s *b);
cube_s masked_cube(const cube_s *cube, const cube_s *mask);
bool compare_cubes(const cube_s *a, const cube_s *b);

cube_s get_edges(const cube_s *cube, face_e color1, face_e color2);
cube_s get_corners(const cube_s *cube, face_e color1, face_e color2, face_e color3);

void init_move_bitrolls();
void apply_move(cube_s *cube, move_s move);
void apply_alg(cube_s *cube, const alg_s *alg);

void print_face(uint32_t face);

void print_cube_map(cube_s cube);
void print_cube_line(cube_s cube);
void print_cube_map_colors(cube_s cube);
void print_cube_line_colors(cube_s cube);

#endif // CUBE_H
