#ifndef LL_STUFF_H
#define LL_STUFF_H

#include "main.h"
#include "cube_alg_table.h"

void LL_print_algs_bigger_than_n(const cube_alg_table_s* ct, size_t n);
bool LL_check_if_1LLL_is_valid(const cube_alg_table_s* ct);
void LL_table_diagnostics(const cube_alg_table_s* ct);

void print_alg_length_frequencies(const cube_alg_table_s* ct);
void LL_find_improvements_to_depth_n(cube_alg_table_s* ct, uint8_t n, size_t start_ind);
cube_alg_table_s* get_very_unique_1LLL_cases(const cube_alg_table_s* ct);
cube_alg_table_s* get_1LLL_from_very_uniq_cases(const cube_alg_table_s* ct);
//void print_improved_1LLL_algs();

#endif