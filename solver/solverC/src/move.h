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

move_list_s* move_list_create(size_t size);
move_list_s* move_list_copy(const move_list_s *src);
void move_list_free(move_list_s *moves);

int move_list_insert(move_list_s *moves, move_s move, size_t index);
int move_list_delete(move_list_s *moves, size_t index);
size_t move_list_lookup(const move_list_s *moves, move_s move);
move_list_s* move_list_from_move_str(const char *move_str);

void move_list_invert(move_list_s *moves);
void move_list_simplify(move_list_s *moves);

void print_move(move_s move);
void print_move_list(const move_list_s *moves);

static const move_s NULL_MOVE = {
    .face = FACE_NULL,
    .turns = 0
};

#endif // MOVE_H
