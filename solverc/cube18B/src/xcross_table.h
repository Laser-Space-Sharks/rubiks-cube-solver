#ifndef XCROSS_TABLE_H
#define XCROSS_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "cube18B.h"

typedef struct xcross_table xcross_table_s;

xcross_table_s* xcross_table_create(size_t size);
bool xcross_table_insert(xcross_table_s *ct, const cube18B_xcross_s *key, const alg_s *moves);
const alg_list_s* cube_table_lookup(const xcross_table_s *ct, const cube18B_xcross_s *cube);
void xcross_table_free(xcross_table_s *ct);
void xcross_table_clear(xcross_table_s *ct);
void xcross_table_print(xcross_table_s *ct);

size_t xcross_table_entries(const xcross_table_s *ct);
size_t xcross_table_size(const xcross_table_s *ct);

#endif // XCROSS_TABLE_H
