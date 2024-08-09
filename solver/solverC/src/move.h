#ifndef MOVE_H
#define MOVE_H

#include <string.h>
#include <stdint.h>

#include "solver.h"
#include "solver_print.h"

typedef struct {
	face_e face;
	int8_t turns;
} move_s;

typedef struct {
	move_s *list;
	size_t length;
	size_t size;
} move_list_s;


// no particular reason for this choice, 4 is just a nice and small power of two
#define MIN_LIST_RESIZE 4

void init_move_list(move_list_s *move_list, size_t size);
void copy_move_list(move_list_s *cpy, move_list_s *src);
int insert_move(move_list_s *move_list, move_s move, size_t index);
int delete_move(move_list_s *move_list, size_t index);
void free_move_list(move_list_s *move_list);

void simplify_move_list(move_list_s* move_list);

void print_move(move_s move);
void print_move_list(move_list_s move_list);
#endif
