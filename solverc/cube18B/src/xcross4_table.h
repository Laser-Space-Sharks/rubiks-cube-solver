#ifndef XCROSS4_TABLE_H
#define XCROSS4_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "cube18B.h"

typedef struct xcross4_table xcross4_table_s;

xcross4_table_s* xcross4_table_create(size_t size);
bool xcross4_table_insert(xcross4_table_s *ct, const cube18B_xcross4_s *key, const alg_s *moves);
const alg_list_s* xcross4_table_lookup(const xcross4_table_s *ct, const cube18B_xcross4_s *cube);
void xcross4_table_free(xcross4_table_s *ct);
void xcross4_table_clear(xcross4_table_s *ct);
void xcross4_table_print(xcross4_table_s *ct);

size_t xcross4_table_entries(const xcross4_table_s *ct);
size_t xcross4_table_size(const xcross4_table_s *ct);

#endif // XCROSS4_TABLE_H
