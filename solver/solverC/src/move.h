#ifndef MOVE_H
#define MOVE_H

#include <string.h>
#include <stdint.h>

#include "main.h"
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

int init_move_list(move_list_s *moves, size_t size);
int copy_move_list(move_list_s *cpy, move_list_s *src);
int insert_move(move_list_s *moves, move_s move, size_t index);
int delete_move(move_list_s *moves, size_t index);
void free_move_list(move_list_s *moves);

void invert_move_list(move_list_s *moves);
int in_move_list(move_list_s *moves, move_s move);
int face_in_move_list(move_list_s *moves, face_e face);

void simplify_move_list(move_list_s *moves);
int move_list_from_move_str(move_list_s *moves, char *move_str);

void print_move(move_s move);
void print_move_list(move_list_s moves);

static const move_s NULL_MOVE = {
    .face = FACE_NULL,
    .turns = 0
};

#endif // MOVE_H
