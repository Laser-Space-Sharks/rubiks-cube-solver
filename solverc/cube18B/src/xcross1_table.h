#ifndef XCROSS1_TABLE_H
#define XCROSS1_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "cube18B.h"

typedef struct xcross1_table xcross1_table_s;

xcross1_table_s* xcross1_table_create(size_t size);
bool xcross1_table_insert(xcross1_table_s *ct, const cube18B_xcross1_s *key, const alg_s *moves);
const alg_list_s* xcross1_table_lookup(const xcross1_table_s *ct, const cube18B_xcross1_s *cube);
void xcross1_table_free(xcross1_table_s *ct);
void xcross1_table_clear(xcross1_table_s *ct);
void xcross1_table_print(xcross1_table_s *ct);

size_t xcross1_table_entries(const xcross1_table_s *ct);
size_t xcross1_table_size(const xcross1_table_s *ct);

#endif // XCROSS1_TABLE_H
