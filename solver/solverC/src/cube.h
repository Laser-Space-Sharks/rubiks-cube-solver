#ifndef CUBE_H
#define CUBE_H

#include "solver.h"
#include "move.h"

void apply_move(cube_s *cube, move_s move);

char get_char(face_e face);
char get_piece(uint64_t face, uint8_t index);
void print_face(uint64_t face);
void print_cube(cube_s cube);

#endif
