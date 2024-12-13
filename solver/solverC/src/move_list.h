#ifndef MOVE_LIST_H
#define MOVE_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "main.h"

typedef struct {
    move_s *list;
    size_t length;
    size_t size;
} move_list_s;

typedef struct {
    move_list_s *list;
    size_t num_algs;
    size_t size;
} alg_list_s;

// no particular reason for this choice, 4 is just a nice and small power of two
#define MIN_LIST_RESIZE 4

move_list_s* move_list_create(size_t size);
move_list_s* move_list_copy(const move_list_s *src);
move_list_s move_list_static_copy(const move_list_s *src);
void move_list_free(move_list_s *moves);

bool move_list_insert(move_list_s *moves, move_s move, size_t index);
bool move_list_delete(move_list_s *moves, size_t index);
size_t move_list_lookup(const move_list_s *moves, move_s move);
move_list_s* move_list_from_move_str(const char *move_str);

void move_list_invert(move_list_s *moves);
void move_list_simplify(move_list_s *moves);
void move_list_rotate_on_y(move_list_s *moves, uint8_t y_turns);

move_s* move_list_concat(move_list_s *dest, const move_list_s *src);
#endif // MOVE_LIST_H
