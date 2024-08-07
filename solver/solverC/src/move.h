#ifndef MOVE_H
#define MOVE_H

#include <string.h>
#include <stdint.h>

#include "solver.h"

// no special reason for the initial size being 16, 
// 16 is just a nice power of two
#define INIT_MOVE_LIST_SIZE 16

typedef struct {
	face_e face;
	int8_t turns;
} move_s;

typedef struct {
	move_s *moves;
	size_t length;
	size_t size;
} move_list_s;

move_list_s init_move_list();
void insert_move(move_list_s *move_list, move_s move, size_t index);
void delete_move(move_list_s *move_list, size_t index);
void free_move_list(move_list_s *move_list);

#endif
