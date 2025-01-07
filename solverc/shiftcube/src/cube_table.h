#ifndef CUBE_TABLE_H
#define CUBE_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "shift_cube.h"

typedef struct cube_table cube_table_s;

cube_table_s* cube_table_create(size_t size);
bool cube_table_insert(cube_table_s *ct, const shift_cube_s *key, const alg_s *moves);
const alg_list_s* cube_table_lookup(const cube_table_s *ct, const shift_cube_s *cube);
void cube_table_free(cube_table_s *ct);
void cube_table_clear(cube_table_s *ct);
void cube_table_print(cube_table_s *ct);
void cube_table_print_algs_bigger_than_n(const cube_table_s* ct, size_t n);
bool cube_table_check_if_1LLL_is_valid(const cube_table_s* ct);
void LL_table_diagnostics(const cube_table_s* ct);

size_t cube_table_entries(const cube_table_s *ct);
size_t cube_table_size(const cube_table_s *ct);

void LL_find_improvements_to_depth_n(const cube_table_s* ct, uint8_t n, size_t start_ind);

#endif // CUBE_TABLE_H
