#ifndef F2L_TABLE_H
#define F2L_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "cube18B.h"

typedef struct F2L_table F2L_table_s;

F2L_table_s* F2L_table_create(size_t size);
bool F2L_table_insert(F2L_table_s *ct, const cube18B_F2L_s *key, const alg_s *moves);
const alg_list_s* F2L_table_lookup(const F2L_table_s *ct, const cube18B_F2L_s *cube);
void F2L_table_free(F2L_table_s *ct);
void F2L_table_clear(F2L_table_s *ct);
void F2L_table_print(F2L_table_s *ct);

size_t F2L_table_entries(const F2L_table_s *ct);
size_t F2L_table_size(const F2L_table_s *ct);

#endif // F2L_TABLE_H
