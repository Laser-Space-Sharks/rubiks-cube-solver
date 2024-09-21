#ifndef CUBE_H
#define CUBE_H

#include "main.h"
#include "move.h"
#include "solver_print.h"

void apply_move(cube_s *cube, move_s move);

void print_face(uint64_t face);
void print_cube(cube_s cube);

#endif // CUBE_H