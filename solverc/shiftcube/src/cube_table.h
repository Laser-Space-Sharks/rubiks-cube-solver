#ifndef CUBE_TABLE_H
#define CUBE_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "shift_cube.h"

typedef struct {
    shift_cube_s key;
    alg_list_s algs;
} entry_s;

typedef struct cube_table {
    size_t entries;
    size_t size;

    entry_s *table;
} cube_table_s;

cube_table_s* cube_table_create(size_t size);
bool cube_table_insert(cube_table_s *ct, const shift_cube_s *key, const alg_s *moves);
bool cube_table_insert_if_new(cube_table_s *ct, const shift_cube_s *key, const alg_s *moves);
const alg_list_s* cube_table_lookup(const cube_table_s *ct, const shift_cube_s *cube);
void cube_table_free(cube_table_s *ct);
void cube_table_clear(cube_table_s *ct);
void cube_table_print(cube_table_s *ct);
void cube_table_print_algs(const cube_table_s *ct);

size_t cube_table_entries(const cube_table_s *ct);
size_t cube_table_size(const cube_table_s *ct);

#endif // CUBE_TABLE_H
