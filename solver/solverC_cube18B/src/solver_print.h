#ifndef SOLVER_PRINT_H
#define SOLVER_PRINT_H

#include "main.h"
#include "alg.h"

// ASCII display utilities
char get_char(face_e face);
char get_piece(uint32_t face, uint8_t index);
void print_piece(uint32_t face, uint8_t index);

void print_move(move_e move);
void print_alg(const alg_s *alg);

#endif // SOLVER_PRINT_H
