#ifndef SOLVER_PRINT_H
#define SOLVER_PRINT_H

#include "main.h"
#include "move_list.h"

// ASCII display utilities
char get_char(face_e face);
char get_piece(uint32_t face, uint8_t index);
void print_piece(uint32_t face, uint8_t index);

void print_move(move_s move);
void print_move_list(const move_list_s *moves);

#endif // SOLVER_PRINT_H
