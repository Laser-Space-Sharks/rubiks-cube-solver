#include "shift_cube.h"
#include "solver_print.h"
#include "cube_table.h"
#include "lookup_tables.h"
#include "solver.h"

typedef struct {
    shift_cube_s key;
    alg_list_s algs;
} entry_s;

typedef struct cube_table {
    size_t entries;
    size_t size;

    entry_s *table;
} cube_table_s;

cube_table_s* cube_table_create(size_t size) {
    cube_table_s *ct = (cube_table_s*)malloc(sizeof(cube_table_s));

    ct->table = (entry_s*)calloc(size, sizeof(entry_s));

    ct->entries = 0;
    ct->size    = size;
    return ct;
}

size_t cube_table_hash(const cube_table_s *ct, const shift_cube_s *key) {
    size_t hash = 0;
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        hash ^= key->state[face];
        hash  = rolq(hash, 17);
    }

    return hash % ct->size;
}

bool cube_table_insert(cube_table_s *ct, const shift_cube_s *key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    size_t hash = cube_table_hash(ct, key);
    size_t index = hash;

    // linear probing
    while (ct->table[index].algs.list != NULL) {
        if (compare_cubes(&(ct->table[index].key), key)) {
            if (ct->table[index].algs.num_algs == ct->table[index].algs.size) {
                size_t new_size = 2*sizeof(alg_s)*ct->table[index].algs.size;
                alg_s *tmp = realloc(ct->table[index].algs.list, new_size);

                if (!tmp) {
                    return false;
                }

                ct->table[index].algs.list = tmp;
                ct->table[index].algs.size *=2;
            }

            ct->table[index].algs.list[ct->table[index].algs.num_algs] = alg_static_copy(moves);
            ct->table[index].algs.num_algs++;

            return true;
        }

        index++;
        if (index == hash) {
            return false;
        }

        if (index >= ct->size) {
            index = 0;
        }
    }


    ct->table[index].key  = *key;

    ct->entries++;

    ct->table[index].algs.list = (alg_s*)malloc(sizeof(alg_s));
    ct->table[index].algs.list[0] = alg_static_copy(moves);

    ct->table[index].algs.num_algs = 1;
    ct->table[index].algs.size = 1;

    return true;
}

static inline size_t cube_table_get_cube_index(const cube_table_s *ct, const shift_cube_s *cube) {
    if (ct == NULL || cube == NULL) {
        return ct->size;
    }

    size_t hash = cube_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].algs.list != NULL) {
        if (compare_cubes(&(ct->table[index].key), cube)) {
            return index;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ct->size) ? 0 : index + 1;
    }

    return ct->size;
}

const alg_list_s* cube_table_lookup(const cube_table_s *ct, const shift_cube_s *cube) {
    size_t index = cube_table_get_cube_index(ct, cube);

    return (index == ct->size) ? NULL : &ct->table[index].algs;
}

void cube_table_clear(cube_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        return;
    }

    for (size_t index = 0; index < ct->size; index++) {
        if (ct->table[index].algs.list != NULL) {
            for (size_t i = 0; i < ct->table[index].algs.num_algs; i++) {
                free(ct->table[index].algs.list[i].moves);
            }

            free(ct->table[index].algs.list);
            ct->table[index].algs.list = NULL;
        }
    }

    ct->entries = 0;
}

void cube_table_free(cube_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        free(ct);
        return;
    }

    cube_table_clear(ct);

    free(ct->table);
    free(ct);
}

void cube_table_print(cube_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            printf("%10zu ", idx);
            print_cube_line_colors(ct->table[idx].key);
            print_alg(&ct->table[idx].algs.list[0]);
        }

        if (idx != 0) {
            if (ct->table[idx].algs.list == NULL && ct->table[idx-1].algs.list != NULL) {
                printf("                                    ...                                    \n");
            }
        }
    }
}

size_t cube_table_entries(const cube_table_s *ct) {
    return ct->entries;
}

size_t cube_table_size(const cube_table_s *ct) {
    return ct->size;
}

void cube_table_print_algs_bigger_than_n(const cube_table_s* ct, size_t n) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list[0].length <= n) continue;
        if (ct->table[idx].algs.list != NULL) {
            printf("%10zu ", idx);
            print_cube_line_colors(ct->table[idx].key);
            print_alg(&ct->table[idx].algs.list[0]);
        }
    }
}

bool cube_table_check_if_1LLL_is_valid(const cube_table_s* ct) {
    bool is_valid = true;
    if (cube_table_entries(ct) != 62208) is_valid = false;
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            if (ct->table[idx].algs.num_algs != 1) is_valid = false;
            shift_cube_s cube_mask = masked_cube(&ct->table[idx].key, &f2l_4mask);
            shift_cube_s solved_mask = masked_cube(&SOLVED_SHIFTCUBE, &f2l_4mask);
            if (!compare_cubes(&cube_mask, &solved_mask)) is_valid = false;
            shift_cube_s test_cube = ct->table[idx].key;
            apply_alg(&test_cube, &ct->table[idx].algs.list[0]);
            if (!compare_cubes(&test_cube, &SOLVED_SHIFTCUBE)) is_valid = false;
        }
    } if (!is_valid) printf("1LLL NOT VALID!\n");
    return is_valid;
}

uint8_t LL_table_get_maxmimum_alg_length(const cube_table_s* ct) {
    uint8_t max = 0;
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            uint8_t candidate = ct->table[idx].algs.list[0].length;
            if (candidate > max) max = candidate;
        }
    } return max;
}

void print_alg_length_frequencies(const cube_table_s* ct) {
    printf("num entries: %zu\n", cube_table_entries(ct));
    uint8_t maxlength = LL_table_get_maxmimum_alg_length(ct);
    size_t counts[maxlength+1];
    for (int i = 0; i < maxlength+1; i++) counts[i] = 0;

    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            counts[ct->table[idx].algs.list[0].length]++;
        }
    }
    printf("maximum length is %hhu", maxlength);
    printf("\nlengths:    ");
    for (uint8_t i = 0; i < maxlength+1; i++) {
        printf("%6hhu", i);
    }
    printf("\nfrequency: [");
    for (uint8_t i = 0; i < maxlength+1; i++) {
        printf("%6zu", counts[i]);
    } printf("]\n");
}

void LL_table_diagnostics(const cube_table_s* ct) {
    if (cube_table_check_if_1LLL_is_valid(ct)) {
        printf("1LLL is valid!\n");
    } print_alg_length_frequencies(ct);
}

void bidirectional_recursion_1LLL_build_end_ct(shift_cube_s *cube, cube_table_s *our_ct, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        if (!cube_table_lookup(our_ct, cube)) {
            cube_table_insert(our_ct, cube, alg);
        }
        return;
    }

    // depth > 0
    if (cube_table_lookup(our_ct, cube) != NULL &&
        cube_table_lookup(our_ct, cube)->list[0].length < alg->length) {
        return;
    }

    move_e prev_move = (alg->length >= 1) ? alg->moves[alg->length-1] : MOVE_NULL;
    move_e prev_prev_move = (alg->length >= 2) ? alg->moves[alg->length - 2] : MOVE_NULL;

    for (move_e move = 0; move < NUM_MOVES; move++) {
        if (move_faces[move] == move_faces[prev_move]) {
            continue;
        }

        if (move_faces[move] == opposite_faces[move_faces[prev_move]] &&
            (move_faces[move] == move_faces[prev_prev_move] || move_faces[move] > move_faces[prev_move])) { continue; } alg_insert(alg, move, alg->length);
        apply_move(cube, move);
        bidirectional_recursion_1LLL_build_end_ct(cube, our_ct, alg, depth - 1);

        // keep going, move didn't pan out
        alg_delete(alg, alg->length-1);
        apply_move(cube, move_inverted[move]); // undo move
    }
}

static alg_s* bidirectional_1LLL_search(const shift_cube_s *start, const cube_table_s* end_ct, uint8_t max_depth) {
    uint8_t start_depth = max_depth/2;

    if (start_depth > MAX_CUBE_TABLE_DEPTH) {
        printf("Attempted to recurse too deep for cube_table\n");
        return NULL;
    }

    if (cube_table_lookup(end_ct, start)) {
        alg_s* end_alg = alg_copy(&cube_table_lookup(end_ct, start)->list[0]);
        alg_invert(end_alg);
        return end_alg;
    }

    alg_s *start_alg = alg_create(start_depth);
    alg_s *end_alg   = alg_create((max_depth+1)/2);

    shift_cube_s start_cube = *start;
    shift_cube_s end_cube   = SOLVED_SHIFTCUBE;

    cube_table_s *start_ct = cube_table_create(cube_table_depth_sizes[start_depth]);

    bool found = false;

    for (uint8_t depth = 0; depth <= start_depth; depth++) {
        if (bidirectional_recursion(&start_cube, start_ct, end_ct, start_alg, depth)) {
            alg_free(end_alg);
            end_alg = alg_copy(&cube_table_lookup(end_ct, &start_cube)->list[0]);
            found = true;
            break;
        }
    }
    cube_table_free(start_ct);
    if (!found) {
        alg_free(start_alg);
        alg_free(end_alg);
        return NULL;
    } else {
        alg_invert(end_alg);
        alg_concat(start_alg, end_alg);
        alg_free(end_alg);
        return start_alg;
    }
}


void LL_find_improvements_to_depth_n(cube_table_s* ct, uint8_t n, size_t start_ind) {
    size_t total_found, total_improved;
    total_found = total_improved = 0;

    cube_table_s *end_ct   = cube_table_create(cube_table_depth_sizes[(n+1)/2]);
    uint8_t end_depth = (n+1)/2;
    shift_cube_s end_cube = SOLVED_SHIFTCUBE;
    alg_s *end_alg   = alg_create((n+1)/2);

    for (uint8_t depth = 0; depth <= end_depth; depth++) {
        bidirectional_recursion_1LLL_build_end_ct(&end_cube, end_ct, end_alg, depth);
    }


    for (size_t idx = start_ind; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            //printf("trying idx %zu: ", idx);
            uint8_t current_length = ct->table[idx].algs.list[0].length;

            alg_s* alg = bidirectional_1LLL_search(&ct->table[idx].key, end_ct, n);
            if (alg == NULL) {
                //printf("\n");
                continue;
            }

            total_found++;
            if (alg->length < current_length) {
                total_improved++;
                free(ct->table[idx].algs.list[0].moves);
                ct->table[idx].algs.list[0] = alg_static_copy(alg);
                printf("At idx %5zu: alg improved to length %2zu with gain %2zu: ", idx, alg->length, current_length - alg->length);
                print_alg(alg);
            } //else {
                //printf("alg found!\n");
            //}
            alg_free(alg);
        }
    }
    printf("# found: %zu\n", total_found);
    printf("# improved: %zu\n", total_improved);
    cube_table_free(end_ct);
    alg_free(end_alg);
}

const cube_table_s* get_very_unique_1LLL_cases(const cube_table_s* ct) {
    cube_table_s* very_uniq_cases = cube_table_create(9257);
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) { //printf("\tline 367\n");
            alg_list_s* alg_family = get_alg_family(&ct->table[idx].algs.list[0]); //printf("\tline 368\n");
            shift_cube_s least_cube = NULL_CUBE;
            alg_s least_alg;
            least_alg.moves = NULL;
            bool already_in = false;
            for (int i = 0; i < 16; i++) {
                shift_cube_s cube = SOLVED_SHIFTCUBE;
                apply_alg(&cube, &alg_family->list[i]); //printf("\tline 374\n");

                if (cube_table_lookup(very_uniq_cases, &cube)) already_in = true; //printf("\tline 376\n");
                if (compare_cubes(&least_cube, &NULL_CUBE) || shiftcube_a_less_than_b(&cube, &least_cube)) { //printf("\tline 377\n");
                    least_cube = cube;
                    free(least_alg.moves);
                    least_alg = alg_static_copy(&alg_family->list[i]); //printf("\tline 379\n");
                } //printf("\tline 380\n");
                free(alg_family->list[i].moves); //printf("\tline 381\n");
            }
            free(alg_family->list); //printf("\tline 383\n");
            free(alg_family); //printf("\tline 384\n");
            if (!already_in) { //printf("\tline 385\n");
                alg_invert(&least_alg);
                cube_table_insert(very_uniq_cases, &least_cube, &least_alg); //printf("\tline 386\n");
            } //printf("\tline 387\n");
            free(least_alg.moves); //printf("\tline 388\n");
        }
    } print_alg_length_frequencies(very_uniq_cases);
    return very_uniq_cases;
} 

const cube_table_s* get_1LLL_from_very_uniq_cases(const cube_table_s* ct) {
    cube_table_s* LL_table = cube_table_create(131009);
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            alg_list_s* alg_family = get_alg_family(&ct->table[idx].algs.list[0]);
            for (int i = 0; i < 16; i++) {
                shift_cube_s cube = SOLVED_SHIFTCUBE;
                apply_alg(&cube, &alg_family->list[i]);
                alg_invert(&alg_family->list[i]);
                if (!cube_table_lookup(LL_table, &cube)) {
                    cube_table_insert(LL_table, &cube, &alg_family->list[i]);
                }
                free(alg_family->list[i].moves);
            }
            free(alg_family->list);
            free(alg_family);
        }
    } print_alg_length_frequencies(LL_table);
    return LL_table;
}

void cube_table_print_algs(const cube_table_s *ct) {
    for (size_t idx = 0; idx < ct->size; idx++) {
        print_alg(&ct->table[idx].algs.list[0]);
    }
}
