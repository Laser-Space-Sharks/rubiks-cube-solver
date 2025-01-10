#ifndef CUBE_ALG_TABLE_H
#define CUBE_ALG_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "shift_cube.h"

typedef struct {
    shift_cube_s key;
    alg_s alg;
} cube_alg_entry_s;

typedef struct cube_alg_table {
    size_t entries;
    size_t size;

    cube_alg_entry_s *table;
} cube_alg_table_s;

cube_alg_table_s* cube_alg_table_create(size_t size);
bool cube_alg_table_overwrite(cube_alg_table_s *ct, const shift_cube_s *key, const alg_s *moves);
bool cube_alg_table_insert_if_new(cube_alg_table_s *ct, const shift_cube_s *key, const alg_s *moves);
const alg_s* cube_alg_table_lookup(const cube_alg_table_s *ct, const shift_cube_s *cube);
void cube_alg_table_clear(cube_alg_table_s *ct);
void cube_alg_table_free(cube_alg_table_s *ct);
void cube_alg_table_print(const cube_alg_table_s *ct);
void cube_alg_table_print_algs(const cube_alg_table_s *ct);

#endif // CUBE_ALG_TABLE_H
