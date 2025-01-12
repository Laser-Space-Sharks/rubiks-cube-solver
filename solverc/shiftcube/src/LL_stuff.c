#include "LL_stuff.h"
#include "main.h"
#include "cube_alg_table.h"
#include "lookup_tables.h"

void LL_print_algs_bigger_than_n(const cube_alg_table_s* ct, size_t n) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.length <= n) continue;
        if (ct->table[idx].alg.moves != NULL) {
            printf("%10zu ", idx);
            print_cube_line_colors(ct->table[idx].key);
            print_alg(&ct->table[idx].alg);
        }
    }
}

bool LL_check_if_1LLL_is_valid(const cube_alg_table_s* ct) {
    bool is_valid = true;
    if (ct->entries != 62208) is_valid = false;
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) {
            shift_cube_s cube_mask = masked_cube(&ct->table[idx].key, &f2l_4mask);
            shift_cube_s solved_mask = masked_cube(&SOLVED_SHIFTCUBE, &f2l_4mask);
            if (!compare_cubes(&cube_mask, &solved_mask)) is_valid = false;
            shift_cube_s test_cube = ct->table[idx].key;
            apply_alg(&test_cube, &ct->table[idx].alg);
            if (!compare_cubes(&test_cube, &SOLVED_SHIFTCUBE)) is_valid = false;
        }
    } if (!is_valid) printf("1LLL NOT VALID!\n");
    return is_valid;
}

uint8_t LL_get_maximimum_alg_length(const cube_alg_table_s* ct) {
    uint8_t max = 0;
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) {
            uint8_t candidate = ct->table[idx].alg.length;
            if (candidate > max) max = candidate;
        }
    } return max;
}

void print_alg_length_frequencies(const cube_alg_table_s* ct) {
    printf("num entries: %zu\n", ct->entries);
    uint8_t maxlength = LL_get_maximimum_alg_length(ct);
    size_t counts[maxlength+1];
    for (int i = 0; i < maxlength+1; i++) counts[i] = 0;

    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) {
            counts[ct->table[idx].alg.length]++;
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

void LL_table_diagnostics(const cube_alg_table_s* ct) {
    if (LL_check_if_1LLL_is_valid(ct)) {
        printf("1LLL is valid!\n");
    } print_alg_length_frequencies(ct);
}


cube_alg_table_s* get_very_unique_1LLL_cases(const cube_alg_table_s* ct) {
    cube_alg_table_s* very_uniq_cases = cube_alg_table_create(9257);
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) { //printf("\tline 367\n");
            alg_list_s* alg_family = get_alg_family(&ct->table[idx].alg); //printf("\tline 368\n");
            shift_cube_s least_cube = NULL_CUBE;
            alg_s least_alg;
            least_alg.moves = NULL;
            bool already_in = false;
            for (int i = 0; i < alg_family->num_algs; i++) {
                shift_cube_s cube = SOLVED_SHIFTCUBE;
                apply_alg(&cube, &alg_family->list[i]); //printf("\tline 374\n");

                if (cube_alg_table_lookup(very_uniq_cases, &cube)) already_in = true; //printf("\tline 376\n");
                if (compare_cubes(&least_cube, &NULL_CUBE) || shiftcube_a_less_than_b(&cube, &least_cube)) { //printf("\tline 377\n");
                    least_cube = cube;
                    free(least_alg.moves);
                    least_alg = alg_static_copy(&alg_family->list[i]); //printf("\tline 379\n");
                } //printf("\tline 380\n");
            }
            alg_list_free(alg_family); //printf("\tline 384\n");
            if (!already_in) { //printf("\tline 385\n");
                alg_invert(&least_alg);
                cube_alg_table_overwrite_if_better(very_uniq_cases, &least_cube, &least_alg); //printf("\tline 386\n");
            } //printf("\tline 387\n");
            free(least_alg.moves); //printf("\tline 388\n");
        }
    } print_alg_length_frequencies(very_uniq_cases);
    return very_uniq_cases;
} 

cube_alg_table_s* get_1LLL_from_very_uniq_cases(const cube_alg_table_s* ct) {
    cube_alg_table_s* LL_table = cube_alg_table_create(131009);
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) {
            alg_list_s* alg_family = get_alg_family(&ct->table[idx].alg);
            for (int i = 0; i < alg_family->num_algs; i++) {
                shift_cube_s cube = SOLVED_SHIFTCUBE;
                apply_alg(&cube, &alg_family->list[i]);
                alg_invert(&alg_family->list[i]);
                cube_alg_table_insert_if_new(LL_table, &cube, &alg_family->list[i]);
            }
            alg_list_free(alg_family);
        }
    } print_alg_length_frequencies(LL_table);
    return LL_table;
}

void bidirectional_recursion_1LLL_build_end_ct(shift_cube_s *cube, cube_alg_table_s *our_ct, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        cube_alg_table_insert_if_new(our_ct, cube, alg);
        return;
    }

    // depth > 0
    if (cube_alg_table_lookup(our_ct, cube) != NULL &&
        cube_alg_table_lookup(our_ct, cube)->length < alg->length) {
        return;
    }

    move_e prev_move = (alg->length >= 1) ? alg->moves[alg->length-1] : MOVE_NULL;
    move_e prev_prev_move = (alg->length >= 2) ? alg->moves[alg->length - 2] : MOVE_NULL;

    for (move_e move = 0; move < NUM_MOVES; move++) {
        if (move_faces[move] == move_faces[prev_move]) {
            continue;
        }

        if (move_faces[move] == opposite_faces[move_faces[prev_move]] &&
            (move_faces[move] == move_faces[prev_prev_move] || move_faces[move] > move_faces[prev_move])) { 
            continue; 
        } 
        
        alg_insert(alg, move, alg->length);
        apply_move(cube, move);
        bidirectional_recursion_1LLL_build_end_ct(cube, our_ct, alg, depth - 1);

        // keep going, move didn't pan out
        alg_delete(alg, alg->length-1);
        apply_move(cube, move_inverted[move]); // undo move
    }
}

int bidirectional_recursion_1LLL(shift_cube_s *cube, cube_alg_table_s *our_ct, const cube_alg_table_s *other_ct, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        cube_alg_table_insert_if_new(our_ct, cube, alg);
        return (cube_alg_table_lookup(other_ct, cube) != NULL);
    }

    if (cube_alg_table_lookup(our_ct, cube) != NULL && 
        cube_alg_table_lookup(our_ct, cube)->length < alg->length) {
        return 0;
    }

    move_e prev_move = (alg->length >= 1) ? alg->moves[alg->length-1] : MOVE_NULL;
    move_e prev_prev_move = (alg->length >= 2) ? alg->moves[alg->length - 2] : MOVE_NULL;

    move_e move = 0;
    while (move < NUM_MOVES) {
        if (move_faces[move] == move_faces[prev_move]) {
            move += 3;
            continue;
        }

        if (move_faces[move] == opposite_faces[move_faces[prev_move]] &&
            (move_faces[move] == move_faces[prev_prev_move] || move_faces[move] > move_faces[prev_move])) {
            move += 3;
            continue;
        }

        alg_append(alg, move);
        apply_move(cube, move);
        if (bidirectional_recursion_1LLL(cube, our_ct, other_ct, alg, depth - 1)) {
            // we did it!
            return 1;
        }

        // keep going, move didn't pan out
        alg_pop(alg);
        apply_move(cube, move_inverted[move]); // undo move
        move++;
    }
    return 0;
}

int bidirectional_recursion_1LLL_dumbFast_DFS(shift_cube_s *cube, const cube_alg_table_s *other_ct, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        return (cube_alg_table_lookup(other_ct, cube) != NULL);
    }

    move_e prev_move = (alg->length >= 1) ? alg->moves[alg->length-1] : MOVE_NULL;
    move_e prev_prev_move = (alg->length >= 2) ? alg->moves[alg->length - 2] : MOVE_NULL;

    move_e move = 0;
    while (move < NUM_MOVES) {
        if (move_faces[move] == move_faces[prev_move]) {
            move += 3;
            continue;
        }

        if (move_faces[move] == opposite_faces[move_faces[prev_move]] &&
            (move_faces[move] == move_faces[prev_prev_move] || move_faces[move] > move_faces[prev_move])) {
            move += 3;
            continue;
        }

        alg_append(alg, move);
        apply_move(cube, move);
        if (bidirectional_recursion_1LLL_dumbFast_DFS(cube, other_ct, alg, depth - 1)) {
            // we did it!
            return 1;
        }

        // keep going, move didn't pan out
        alg_pop(alg);
        apply_move(cube, move_inverted[move]); // undo move
        move++;
    }
    return 0;
}

static alg_s* bidirectional_1LLL_search(const shift_cube_s *start, const cube_alg_table_s* end_ct, size_t end_depth, size_t start_depth) {
    //if (start_depth > MAX_CUBE_TABLE_DEPTH) {
    //    printf("Attempted to recurse too deep for cube_table\n");
    //    return NULL;
    //}

    if (cube_alg_table_lookup(end_ct, start)) {
        alg_s* end_alg = alg_copy(cube_alg_table_lookup(end_ct, start));
        alg_invert(end_alg);
        return end_alg;
    }

    alg_s *start_alg = alg_create(start_depth);
    alg_s *end_alg   = alg_create(end_depth);

    shift_cube_s start_cube = *start;
    shift_cube_s end_cube   = SOLVED_SHIFTCUBE;

    bool found = false;
    if (start_depth >= 7) {
        uint8_t depth = start_depth;
        if (bidirectional_recursion_1LLL_dumbFast_DFS(&start_cube, end_ct, start_alg, depth)) {
            alg_free(end_alg);
            end_alg = alg_copy(cube_alg_table_lookup(end_ct, &start_cube));
            found = true;
        }
    } else {
        cube_alg_table_s *start_ct = cube_alg_table_create(cube_table_depth_sizes[start_depth]);
        for (uint8_t depth = 0; depth <= start_depth; depth++) {
            if (bidirectional_recursion_1LLL(&start_cube, start_ct, end_ct, start_alg, depth)) {
                alg_free(end_alg);
                end_alg = alg_copy(cube_alg_table_lookup(end_ct, &start_cube));
                found = true;
                break;
            }
        }
        cube_alg_table_free(start_ct);
    }

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


void LL_find_improvements_to_depth_n(cube_alg_table_s* ct, uint8_t n, size_t start_ind) {
    size_t total_found, total_improved;
    total_found = total_improved = 0;
    size_t end_depth = (n+1)/2;
    if (end_depth > 7) end_depth = 7;
    size_t start_depth = n-end_depth;

    cube_alg_table_s *end_ct   = cube_alg_table_create(cube_table_depth_sizes[end_depth]);
    shift_cube_s end_cube = SOLVED_SHIFTCUBE;
    alg_s *end_alg   = alg_create(end_depth);

    for (uint8_t depth = 0; depth <= end_depth; depth++) {
        bidirectional_recursion_1LLL_build_end_ct(&end_cube, end_ct, end_alg, depth);
    }

    size_t tried = 0;
    size_t total_algs_to_try = 0;
    for (size_t idx = start_ind; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves == NULL) continue;
        if (ct->table[idx].alg.length <= n) continue;
        total_algs_to_try++;
    }
    for (size_t idx = start_ind; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves == NULL) continue;
        if (ct->table[idx].alg.length <= n) continue;

        uint8_t current_length = ct->table[idx].alg.length;

        alg_s* alg = bidirectional_1LLL_search(&ct->table[idx].key, end_ct, end_depth, start_depth);
        tried++;
        if (alg == NULL) {
            continue;
        } //else {
            //printf("WE FOUND index %zu at depth %zu\n", idx, n);
        //}

        total_found++;
        if (alg->length < current_length) {
            total_improved++;
            free(ct->table[idx].alg.moves);
            ct->table[idx].alg = alg_static_copy(alg);
            printf("On try %4zu/%4zu: alg improved to length %2hhu with gain %2hhu: ", tried, total_algs_to_try, alg->length, current_length - alg->length);
            print_alg(alg);
        }
        alg_free(alg);
    }
    printf("# found/improved: %zu\n", total_improved);
    cube_alg_table_free(end_ct);
    alg_free(end_alg);
}
/*
void print_improved_1LLL_algs() {
    cube_alg_table_s* LL_table = gen_last_layer_table();
    print_alg_length_frequencies(LL_table); // will print
    cube_alg_table_s* uniq_cases = get_very_unique_1LLL_cases(LL_table); // will print
    alg_list_s* improvements = alg_list_from_file(LL_IMPROVEMENTS_PATH);
    for (size_t idx = 0; idx < improvements->num_algs; idx++) {
        shift_cube_s cube = SOLVED_SHIFTCUBE;
        alg_invert(&improvements->list[idx]);
        apply_alg(&cube, &improvements->list[idx]);
        alg_invert(&improvements->list[idx]);
        cube_alg_table_overwrite_if_better(uniq_cases, &cube, &improvements->list[idx]);
    }
    alg_list_free(improvements);
    cube_alg_table_free(LL_table);
    print_alg_length_frequencies(uniq_cases); // will print
    LL_table = get_1LLL_from_very_uniq_cases(uniq_cases); // will print
    LL_check_if_1LLL_is_valid(LL_table);
    cube_alg_table_print_algs(LL_table);
    cube_alg_table_free(LL_table);
    cube_alg_table_free(uniq_cases);
}*/
