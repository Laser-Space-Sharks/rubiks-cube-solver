#ifndef CUBE_H
#define CUBE_H

#include <stdbool.h>

#include "main.h"
#include "solver_print.h"

shift_cube_s* cube_copy(const shift_cube_s *cube);
shift_cube_s ored_cube(const shift_cube_s *a, const shift_cube_s *b);
shift_cube_s masked_cube(const shift_cube_s *cube, const shift_cube_s *mask);
bool compare_cubes(const shift_cube_s *a, const shift_cube_s *b);

shift_cube_s get_edges(const shift_cube_s *cube, face_e color1, face_e color2);
shift_cube_s get_corners(const shift_cube_s *cube, face_e color1, face_e color2, face_e color3);

void init_move_bitrolls();
void old_apply_move(shift_cube_s *c, move_s m);
void apply_move(shift_cube_s *c, move_e m);
void apply_alg(shift_cube_s *cube, const alg_s *alg);

void print_face(uint32_t face);

void print_cube_map(shift_cube_s cube);
void print_cube_line(shift_cube_s cube);
void print_cube_map_colors(shift_cube_s cube);
void print_cube_line_colors(shift_cube_s cube);

#endif // CUBE_H
