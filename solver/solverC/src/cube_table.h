#ifndef CUBE_HASH_H
#define CUBE_HASH_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "move_list.h"

typedef struct cube_table cube_table_s;

cube_table_s* cube_table_create(size_t size);
bool cube_table_insert(cube_table_s *ct, const cube_s *cube, const move_list_s *moves);
move_list_s* cube_table_lookup(const cube_table_s *ct, const cube_s *cube);
void cube_table_free(cube_table_s *ct);
void cube_table_print(cube_table_s *ct);

size_t cube_table_entries(const cube_table_s *ct);
size_t cube_table_size(const cube_table_s *ct);

#endif // CUBE_HASH_H
