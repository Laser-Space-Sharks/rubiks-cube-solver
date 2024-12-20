#ifndef LL_TABLE_H
#define LL_TABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "main.h"
#include "alg.h"
#include "cube18B.h"

typedef struct LL_table LL_table_s;

LL_table_s* LL_table_create(size_t size);
bool LL_table_insert(LL_table_s *ct, const cube18B_1LLL_s *key, const alg_s *moves);
const alg_s* LL_table_lookup(const LL_table_s *ct, const cube18B_1LLL_s *cube);
void LL_table_free(LL_table_s *ct);
void LL_table_print(LL_table_s *ct);

size_t LL_table_entries(const LL_table_s *ct);
size_t LL_table_size(const LL_table_s *ct);

#endif // LL_TABLE_H
