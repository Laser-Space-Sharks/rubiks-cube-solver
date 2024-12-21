#define _GNU_SOURCE

#include "solver.h"

#include "cube18B.h"
#include "lookup_tables.h"
#include "move.h"
#include "translators.h"

#include <stdio.h>
#include <sys/types.h>

static xcross4_table_s *xcross4_start_ct = NULL;
static xcross4_table_s *xcross4_end_ct   = NULL;

bool init_solver4() {
    xcross4_start_ct = xcross4_table_create(6008461);
    xcross4_end_ct   = xcross4_table_create(6008461);
    if (!xcross4_start_ct || !xcross4_end_ct) {
        return false;
    }

    return true;
}

void cleanup_solver4() {
    xcross4_table_free(xcross4_start_ct);
    xcross4_table_free(xcross4_end_ct);
}

static xcross1_table_s *xcross1_start_ct = NULL;
static xcross1_table_s *xcross1_end_ct   = NULL;

bool init_solver1() {
    xcross1_start_ct = xcross1_table_create(cube_table_depth_sizes[5]);
    xcross1_end_ct   = xcross1_table_create(cube_table_depth_sizes[5]);
    if (!xcross1_start_ct || !xcross1_end_ct) {
        return false;
    }

    return true;
}

void cleanup_solver1() {
    xcross1_table_free(xcross1_start_ct);
    xcross1_table_free(xcross1_end_ct);
}

int bidirectional_recursion_best_of_each(
        cube18B_xcross4_s *cube, 
        xcross4_table_s *our_ct, 
        const xcross4_table_s *other_ct, 
        alg_s *alg, 
        bool* pairs_done,
        cube_list_s** cube_lists,
        uint8_t depth
    ) {
    if (depth == 0) {
        for (uint8_t pair = 0; pair < 4; pair++) {
            if (!pairs_done[pair]) {
                cube18B_xcross4_s x = *cube;
                cube18B_xcross4_maskOnPair(&x, pair);
                if (xcross4_table_lookup(other_ct, &x) != NULL && 
                    xcross4_table_lookup(our_ct, &x) == NULL) {
                    cube_list_append(cube_lists[pair], &x);
                }
                xcross4_table_insert(our_ct, &x, alg);
            }
        }
        return 0;
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
        cube18B_xcross4_apply_move(cube, move);

        bidirectional_recursion_best_of_each(cube, our_ct, other_ct, alg, pairs_done, cube_lists, depth - 1);
        
        alg_delete(alg, alg->length-1);
        cube18B_xcross4_apply_move(cube, move_inverted[move]); // undo move
    }
    return 0;
}

static void xcross_search_best_of_each(const cube18B_xcross4_s *start, alg_list_s* xsolves) {
    if (!xcross4_start_ct || !xcross4_end_ct) {
        printf("Didn't have the tables initialized :(\n");
        return;
    }

    alg_s *start_alg = alg_create(5);
    alg_s *end_alg   = alg_create(5);

    cube18B_xcross4_s start_cube = *start;
    cube18B_xcross4_s end_cube   = SOLVED_CUBE18B_XCROSS4;
    //printf("=============STARTING XCROSS===========\n");
    //printf("START_CUBE: \n");
    //print_cube_map_colors(start_cube);
    //printf("END_CUBE: \n");
    //print_cube_map_colors(end_cube);
    bool pairs_done[4] = { false };
    cube_list_s* cube_lists[4] = {
        cube_list_create(4),
        cube_list_create(4),
        cube_list_create(4),
        cube_list_create(4),
    };
    bool done;
    for (uint8_t depth = 0; depth <= 5; depth++) {
        bidirectional_recursion_best_of_each(&start_cube, xcross4_start_ct, xcross4_end_ct, start_alg, pairs_done, cube_lists, depth);
        done = true;
        for (uint8_t pair = 0; pair < 4; pair++) {
            if (cube_lists[pair]->length > 0) {
                pairs_done[pair] = true;
            } else done = false;
        } if (done) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");

        bidirectional_recursion_best_of_each(&end_cube, xcross4_end_ct, xcross4_start_ct, end_alg, pairs_done, cube_lists, depth);
        done = true;
        for (uint8_t pair = 0; pair < 4; pair++) {
            if (cube_lists[pair]->length > 0) {
                pairs_done[pair] = true;
            } else done = false;
        } if (done) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");
    }
    for (uint8_t pair = 0; pair < 4; pair++) {
        for (size_t i = 0; i < cube_lists[pair]->length; i++) {
            cube18B_xcross4_s cube = cube_lists[pair]->cubes[i];
            const alg_list_s* alg_list1 = xcross4_table_lookup(xcross4_start_ct, &cube);
            const alg_list_s* alg_list2 = xcross4_table_lookup(xcross4_end_ct, &cube);
            for (size_t alg1 = 0; alg1 < alg_list1->num_algs; alg1++) {
                for (size_t alg2 = 0; alg2 < alg_list2->num_algs; alg2++) {
                    alg_s start_algo = alg_static_copy(&(alg_list1->list[alg1]));
                    alg_s end_algo = alg_static_copy(&(alg_list2->list[alg2]));
                    alg_invert(&end_algo);
                    alg_concat(&start_algo, &end_algo);
                    alg_list_append(xsolves, &start_algo);
                }
            }
        }
    }
}

int bidirectional_recursion_best_of_any(
        cube18B_xcross4_s *cube, 
        xcross4_table_s *our_ct, 
        const xcross4_table_s *other_ct, 
        alg_s *alg, 
        cube_list_s* cube_list,
        uint8_t depth
    ) {
    if (depth == 0) {
        for (uint8_t pair = 0; pair < 4; pair++) {
            cube18B_xcross4_s x = *cube;
            cube18B_xcross4_maskOnPair(&x, pair);
            if (xcross4_table_lookup(other_ct, &x) != NULL && 
                xcross4_table_lookup(our_ct, &x) == NULL) {
                cube_list_append(cube_list, &x);
            }
            xcross4_table_insert(our_ct, &x, alg);
        }
        return 0;
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
        cube18B_xcross4_apply_move(cube, move);

        bidirectional_recursion_best_of_any(cube, our_ct, other_ct, alg, cube_list, depth - 1);
        
        alg_delete(alg, alg->length-1);
        cube18B_xcross4_apply_move(cube, move_inverted[move]); // undo move
    }
    return 0;
}

static void xcross_search_best_of_any(const cube18B_xcross4_s *start, alg_list_s* xsolves) {
    if (!xcross4_start_ct || !xcross4_end_ct) {
        printf("Didn't have the tables initialized :(\n");
        return;
    }

    alg_s *start_alg = alg_create(5);
    alg_s *end_alg   = alg_create(5);

    cube18B_xcross4_s start_cube = *start;
    cube18B_xcross4_s end_cube   = SOLVED_CUBE18B_XCROSS4;
    //printf("=============STARTING XCROSS===========\n");
    //printf("START_CUBE: \n");
    //print_cube_map_colors(start_cube);
    //printf("END_CUBE: \n");
    //print_cube_map_colors(end_cube);
    cube_list_s* cube_list = cube_list_create(8);

    for (uint8_t depth = 0; depth <= 5; depth++) {
        bidirectional_recursion_best_of_any(&start_cube, xcross4_start_ct, xcross4_end_ct, start_alg, cube_list, depth);
        if (cube_list->length > 0) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");

        bidirectional_recursion_best_of_any(&end_cube, xcross4_end_ct, xcross4_start_ct, end_alg, cube_list, depth);
        if (cube_list->length > 0) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");
    }

    for (size_t i = 0; i < cube_list->length; i++) {
        cube18B_xcross4_s cube = cube_list->cubes[i];
        const alg_list_s* alg_list1 = xcross4_table_lookup(xcross4_start_ct, &cube);
        const alg_list_s* alg_list2 = xcross4_table_lookup(xcross4_end_ct, &cube);
        for (size_t alg1 = 0; alg1 < alg_list1->num_algs; alg1++) {
            for (size_t alg2 = 0; alg2 < alg_list2->num_algs; alg2++) {
                alg_s start_algo = alg_static_copy(&(alg_list1->list[alg1]));
                alg_s end_algo = alg_static_copy(&(alg_list2->list[alg2]));
                alg_invert(&end_algo);
                alg_concat(&start_algo, &end_algo);
                alg_list_append(xsolves, &start_algo);
            }
        }
    }
}
int bidirectional_recursion_first_of_each_together(
        cube18B_xcross4_s *cube, 
        xcross4_table_s *our_ct, 
        const xcross4_table_s *other_ct, 
        alg_s *alg, 
        bool* pairs_done,
        cube_list_s* cube_list,
        uint8_t depth
    ) {
    if (depth == 0) {
        bool done = true;
        for (uint8_t pair = 0; pair < 4; pair++) {
            if (!pairs_done[pair]) {
                cube18B_xcross4_s x = *cube;
                cube18B_xcross4_maskOnPair(&x, pair);
                if (xcross4_table_lookup(other_ct, &x) != NULL && 
                    xcross4_table_lookup(our_ct, &x) == NULL) {
                    cube_list_append(cube_list, &x);
                    pairs_done[pair] = true;
                }
                xcross4_table_insert(our_ct, &x, alg);
                done = false;
            }
        }
        xcross4_table_insert(our_ct, cube, alg);
        return done;
    }

    if (xcross4_table_lookup(our_ct, cube) != NULL && xcross4_table_lookup(our_ct, cube)->list[0].length < alg->length) {
        return 0;
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
        cube18B_xcross4_apply_move(cube, move);

        if (bidirectional_recursion_first_of_each_together(cube, our_ct, other_ct, alg, pairs_done, cube_list, depth - 1)) {
            return 1;
        }
        
        alg_delete(alg, alg->length-1);
        cube18B_xcross4_apply_move(cube, move_inverted[move]); // undo move
    }
    return 0;
}

static void xcross_search_first_of_each_together(const cube18B_xcross4_s *start, alg_list_s* xsolves) {
    if (!xcross4_start_ct || !xcross4_end_ct) {
        printf("Didn't have the tables initialized :(\n");
        return;
    }

    alg_s *start_alg = alg_create(5);
    alg_s *end_alg   = alg_create(5);

    cube18B_xcross4_s start_cube = *start;
    cube18B_xcross4_s end_cube   = SOLVED_CUBE18B_XCROSS4;
    //printf("=============STARTING XCROSS===========\n");
    //printf("START_CUBE: \n");
    //print_cube_map_colors(start_cube);
    //printf("END_CUBE: \n");
    //print_cube_map_colors(end_cube);
    bool pairs_done[4] = { false };
    cube_list_s* cube_list = cube_list_create(8);

    for (uint8_t depth = 0; depth <= 5; depth++) {
        bidirectional_recursion_first_of_each_together(&start_cube, xcross4_start_ct, xcross4_end_ct, start_alg, pairs_done, cube_list, depth);
        if (cube_list->length == 4) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");

        bidirectional_recursion_first_of_each_together(&end_cube, xcross4_end_ct, xcross4_start_ct, end_alg, pairs_done, cube_list, depth);
        if (cube_list->length == 4) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");
    }

    for (size_t i = 0; i < cube_list->length; i++) {
        cube18B_xcross4_s cube = cube_list->cubes[i];
        const alg_list_s* alg_list1 = xcross4_table_lookup(xcross4_start_ct, &cube);
        const alg_list_s* alg_list2 = xcross4_table_lookup(xcross4_end_ct, &cube);
        for (size_t alg1 = 0; alg1 < alg_list1->num_algs; alg1++) {
            for (size_t alg2 = 0; alg2 < alg_list2->num_algs; alg2++) {
                alg_s start_algo = alg_static_copy(&(alg_list1->list[alg1]));
                alg_s end_algo = alg_static_copy(&(alg_list2->list[alg2]));
                alg_invert(&end_algo);
                alg_concat(&start_algo, &end_algo);
                alg_list_append(xsolves, &start_algo);
            }
        }
    }
}
int bidirectional_recursion_first_of_any(
        cube18B_xcross4_s *cube, 
        xcross4_table_s *our_ct, 
        const xcross4_table_s *other_ct, 
        alg_s *alg, 
        cube_list_s* cube_list,
        uint8_t depth
    ) {
    if (depth == 0) {
        bool done = false;
        for (uint8_t pair = 0; pair < 4; pair++) {
            cube18B_xcross4_s x = *cube;
            cube18B_xcross4_maskOnPair(&x, pair);
            if (xcross4_table_lookup(other_ct, &x) != NULL && 
                xcross4_table_lookup(our_ct, &x) == NULL) {
                cube_list_append(cube_list, &x);
                done = true;
            }
            xcross4_table_insert(our_ct, &x, alg);
        }
        xcross4_table_insert(our_ct, cube, alg);
        return done;
    }

    if (xcross4_table_lookup(our_ct, cube) != NULL && xcross4_table_lookup(our_ct, cube)->list[0].length < alg->length) {
        return 0;
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
        cube18B_xcross4_apply_move(cube, move);

        if (bidirectional_recursion_first_of_any(cube, our_ct, other_ct, alg, cube_list, depth - 1)) {
            return 1;
        }
        
        alg_delete(alg, alg->length-1);
        cube18B_xcross4_apply_move(cube, move_inverted[move]); // undo move
    }
    return 0;
}

static void xcross_search_first_of_any(const cube18B_xcross4_s *start, alg_list_s* xsolves) {
    if (!xcross4_start_ct || !xcross4_end_ct) {
        printf("Didn't have the tables initialized :(\n");
        return;
    }

    alg_s *start_alg = alg_create(5);
    alg_s *end_alg   = alg_create(5);

    cube18B_xcross4_s start_cube = *start;
    cube18B_xcross4_s end_cube   = SOLVED_CUBE18B_XCROSS4;
    //printf("=============STARTING XCROSS===========\n");
    //printf("START_CUBE: \n");
    //print_cube_map_colors(start_cube);
    //printf("END_CUBE: \n");
    //print_cube_map_colors(end_cube);
    cube_list_s* cube_list = cube_list_create(8);

    for (uint8_t depth = 0; depth <= 5; depth++) {
        bidirectional_recursion_first_of_any(&start_cube, xcross4_start_ct, xcross4_end_ct, start_alg, cube_list, depth);
        if (cube_list->length > 0) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");

        bidirectional_recursion_first_of_any(&end_cube, xcross4_end_ct, xcross4_start_ct, end_alg, cube_list, depth);
        if (cube_list->length > 0) break;
        if (!compare_cube18B_xcross4(start, &start_cube) || !compare_cube18B_xcross4(&SOLVED_CUBE18B_XCROSS4, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");
    }

    for (size_t i = 0; i < cube_list->length; i++) {
        cube18B_xcross4_s cube = cube_list->cubes[i];
        const alg_list_s* alg_list1 = xcross4_table_lookup(xcross4_start_ct, &cube);
        const alg_list_s* alg_list2 = xcross4_table_lookup(xcross4_end_ct, &cube);
        for (size_t alg1 = 0; alg1 < alg_list1->num_algs; alg1++) {
            for (size_t alg2 = 0; alg2 < alg_list2->num_algs; alg2++) {
                alg_s start_algo = alg_static_copy(&(alg_list1->list[alg1]));
                alg_s end_algo = alg_static_copy(&(alg_list2->list[alg2]));
                alg_invert(&end_algo);
                alg_concat(&start_algo, &end_algo);
                alg_list_append(xsolves, &start_algo);
            }
        }
    }
}

int bidirectional_recursion_first_of_each_separate(cube18B_xcross1_s *cube, xcross1_table_s *our_ct, xcross1_table_s *other_ct, alg_s *alg, uint8_t depth) {
    //if (alg->length > 5) {
    //    printf("ALG GOT BIGGER THAN 5 MOVES!!!! OH THE HORRORS!!!\n");
    //}
    if (depth == 0) {
        if (!xcross1_table_lookup(our_ct, cube)) {
            xcross1_table_insert(our_ct, cube, alg);
        }

        return (xcross1_table_lookup(other_ct, cube) != NULL);
    }

    // depth > 0
    if (xcross1_table_lookup(our_ct, cube) != NULL &&
        xcross1_table_lookup(our_ct, cube)->list[0].length < alg->length) {
        return 0;

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
        //uint8_t len0 = alg->length;

        alg_insert(alg, move, alg->length);
        //uint8_t len1 = alg->length;
        cube18B_xcross1_apply_move(cube, move);
        if (bidirectional_recursion_first_of_each_separate(cube, our_ct, other_ct, alg, depth - 1)) {
            // we did it!
            return 1;
        }

        // keep going, move didn't pan out
        alg_delete(alg, alg->length-1);
        //uint8_t len2 = alg->length;
        cube18B_xcross1_apply_move(cube, move_inverted[move]); // undo move
        //if (len1 != len0+1) {
        //    printf("THE PROBLEM IS THE INSERTION\n");
        //}
        //if (len2 != len1-1) {
        //    printf("THE PROBLEM IS THE DELETION\n");
        //}
    }
    return 0;
}

static void xcross_search_first_of_each_separate(const cube18B_xcross4_s *start, alg_list_s* xsolves) {
    if (!xcross1_start_ct || !xcross1_end_ct) {
        printf("Didn't have the tables initialized :(\n");
        return;
    }

    for (uint8_t pair = 0; pair < 4; pair++) {
        alg_s *start_alg = alg_create(5);
        alg_s *end_alg   = alg_create(5);

        cube18B_xcross1_s start_cube = cube18B_xcross4_to_xcross1(start, pair);
        cube18B_xcross1_s end_cube   = cube18B_xcross4_to_xcross1(&SOLVED_CUBE18B_XCROSS4, pair);
        //printf("=============STARTING XCROSS===========\n");
        //printf("START_CUBE: \n");
        //print_cube_map_colors(start_cube);
        //printf("END_CUBE: \n");
        //print_cube_map_colors(end_cube);
        for (uint8_t depth = 0; depth <= 5; depth++) {
            //printf("LENGTH OF START_ALG IS %zu\n", start_alg->length);
            if (bidirectional_recursion_first_of_each_separate(&start_cube, xcross1_start_ct, xcross1_end_ct, start_alg, depth)) {
                alg_free(end_alg);
                end_alg = alg_copy(&xcross1_table_lookup(xcross1_end_ct, &start_cube)->list[0]);
                break;
            }

            //printf("LENGTH OF END_ALG IS %zu\n", end_alg->length);
            if (bidirectional_recursion_first_of_each_separate(&end_cube, xcross1_end_ct, xcross1_start_ct, end_alg, depth)) {
                alg_free(start_alg);
                start_alg = alg_copy(&xcross1_table_lookup(xcross1_start_ct, &end_cube)->list[0]);
                break;
            }
        }
        alg_invert(end_alg);
        alg_concat(start_alg, end_alg);

        //print_alg(start_alg);

        alg_list_append(xsolves, start_alg);

        alg_free(end_alg);
        alg_free(start_alg);
        xcross1_table_clear(xcross1_start_ct);
        xcross1_table_clear(xcross1_end_ct);
    }
}


static void last_layer_stage(const cube18B_1LLL_s *LL_portion, alg_s **best, const alg_s *xsolve,
                             const alg_s *f2l_solve, const LL_table_s *ll_table) {
    //printf("Entered LL_stage()\n");
    alg_s *solve = alg_copy(xsolve);
    alg_concat(solve, f2l_solve);

    const alg_s *last_layer_alg = LL_table_lookup(ll_table, LL_portion);
    if (last_layer_alg != NULL) {
        alg_concat(solve, last_layer_alg);
    }

    alg_simplify(solve);

    if (*best && (*best)->length <= solve->length) {
        //printf("best_solve was: \n");
        //print_alg(*best);
        alg_free(solve);
        return;
    }
    //printf("\t\t\t\tFOUND NEW BEST SOLVE: ");
    //print_alg(solve);

    alg_free(*best);
    *best = solve;
}

static void f2l_stage(cube18B_F2L_s F2L_portion, cube18B_1LLL_s LL_portion, alg_s **best, const alg_s *xsolve,
                      alg_s *f2l_solve, const F2L_table_s *f2l_table,
                      const LL_table_s *ll_table, uint8_t depth) {
    //if (depth == 2) {
    //    printf("made it to depth 2\n");
    //} 

    // we solved F2L! Proceed to the last layer
    if (compare_cube18B_F2L(&F2L_portion, &SOLVED_CUBE18B_F2L)) {
        last_layer_stage(&LL_portion, best, xsolve, f2l_solve, ll_table);
        return;
    }
    //if (depth == 0) printf("5TH PAIR?!\n");

    for (uint8_t pair = 0; pair < 4; pair++) {
        cube18B_F2L_s solved_mask = SOLVED_CUBE18B_F2L;
        cube18B_F2L_s pair_mask = F2L_portion;
        cube18B_F2L_maskOnPair(&solved_mask, pair);
        cube18B_F2L_maskOnPair(&pair_mask, pair);

        if (compare_cube18B_F2L(&solved_mask, &pair_mask)) continue;

        const alg_list_s *pair_algs = F2L_table_lookup(f2l_table, &pair_mask);
        if (!pair_algs) {
            cube18B_s cube = {
                .cubies = {
                    SOLVED_CUBIES[0],
                    SOLVED_CUBIES[1],
                    SOLVED_CUBIES[2],
                    SOLVED_CUBIES[3],
                    F2L_portion.cubies[0],
                    F2L_portion.cubies[1],
                    F2L_portion.cubies[2],
                    F2L_portion.cubies[3],
                    F2L_portion.cubies[4],
                    F2L_portion.cubies[5],
                    F2L_portion.cubies[6],
                    F2L_portion.cubies[7],
                    LL_portion.cubies[0],
                    LL_portion.cubies[1],
                    LL_portion.cubies[2],
                    LL_portion.cubies[3],
                    LL_portion.cubies[4],
                    LL_portion.cubies[5],
                }
            };
            shift_cube_s shiftcube = shiftCube_from_cube18B(&cube);
            printf("THERE ARE NO PAIR ALGS FOR PAIR %hhu at depth %hhu\n", pair, depth);
            print_cube18B_F2L(&F2L_portion);
            print_cube18B_F2L(&pair_mask);
            print_cube18B_F2L(&SOLVED_CUBE18B_F2L);
            print_cube_map_colors(shiftcube);
            printf("THE XCROSS SOLVE WAS: ");
            print_alg(xsolve);
            printf("------------------\n");
            return;
        }

        for (size_t alg = 0; alg < pair_algs->num_algs; alg++) {
            //if (compare_algs(xsolve, "B2 R B' L' F L F") && compare_algs(f2l_solve, "L2 B L U' B' L B2 D B' U B D' B2")) {
            //    print_cube_map_colors(cube);
            //    print_alg(&(pair_algs->list[alg]));
            //
            alg_s algo = pair_algs->list[alg];
            cube18B_F2L_s new_F2L_portion = F2L_portion;
            //printf("Applying F2L alg on depth %hhu:\n", depth);
            //print_alg(&algo);
            if (F2L_portion.cubies[0] != SOLVED_CUBE18B_F2L.cubies[0] || F2L_portion.cubies[1] != SOLVED_CUBE18B_F2L.cubies[1]) {
                new_F2L_portion.cubies[0] = apply_alg_to_cubie(new_F2L_portion.cubies[0], &algo);
                new_F2L_portion.cubies[1] = apply_alg_to_cubie(new_F2L_portion.cubies[1], &algo);
            } if (F2L_portion.cubies[2] != SOLVED_CUBE18B_F2L.cubies[2] || F2L_portion.cubies[3] != SOLVED_CUBE18B_F2L.cubies[3]) {
                new_F2L_portion.cubies[2] = apply_alg_to_cubie(new_F2L_portion.cubies[2], &algo);
                new_F2L_portion.cubies[3] = apply_alg_to_cubie(new_F2L_portion.cubies[3], &algo);
            } if (F2L_portion.cubies[4] != SOLVED_CUBE18B_F2L.cubies[4] || F2L_portion.cubies[5] != SOLVED_CUBE18B_F2L.cubies[5]) {
                new_F2L_portion.cubies[4] = apply_alg_to_cubie(new_F2L_portion.cubies[4], &algo);
                new_F2L_portion.cubies[5] = apply_alg_to_cubie(new_F2L_portion.cubies[5], &algo);
            } if (F2L_portion.cubies[6] != SOLVED_CUBE18B_F2L.cubies[6] || F2L_portion.cubies[7] != SOLVED_CUBE18B_F2L.cubies[7]) {
                new_F2L_portion.cubies[6] = apply_alg_to_cubie(new_F2L_portion.cubies[6], &algo);
                new_F2L_portion.cubies[7] = apply_alg_to_cubie(new_F2L_portion.cubies[7], &algo);
            }
            //for (uint8_t i = 4; i > depth; i--) {
            //    printf("\t");
            //} print_cube18B_F2L(&new_F2L_portion);
            cube18B_1LLL_s new_LL_portion = LL_portion;
            cube18B_1LLL_apply_alg(&new_LL_portion, &algo);
            size_t old_len = f2l_solve->length;
            alg_concat(f2l_solve, &algo);
            f2l_stage(new_F2L_portion, new_LL_portion, best, xsolve, f2l_solve, f2l_table, ll_table, depth-1);
            f2l_solve->length -= f2l_solve->length - old_len;
        }
    }
}

static bool test_xcross_alg(const cube18B_xcross4_s* xpuzzle, const alg_s* xsolve) {
    cube18B_xcross4_s puzzle = *xpuzzle;
    cube18B_xcross4_apply_alg(&puzzle, xsolve);
    return (
        puzzle.cubies[0] == SOLVED_CUBIES[0] && 
        puzzle.cubies[1] == SOLVED_CUBIES[1] &&
        puzzle.cubies[2] == SOLVED_CUBIES[2] &&
        puzzle.cubies[3] == SOLVED_CUBIES[3] &&
        (
            (puzzle.cubies[4] == SOLVED_CUBIES[4] && puzzle.cubies[5] == SOLVED_CUBIES[5]) ||
            (puzzle.cubies[6] == SOLVED_CUBIES[6] && puzzle.cubies[7] == SOLVED_CUBIES[7]) ||
            (puzzle.cubies[8] == SOLVED_CUBIES[8] && puzzle.cubies[9] == SOLVED_CUBIES[9]) ||
            (puzzle.cubies[10] == SOLVED_CUBIES[10] && puzzle.cubies[11] == SOLVED_CUBIES[11])
        )
    );
}
static void xcross_stage(cube18B_s cube, alg_s **best,
                         const F2L_table_s *f2l_table, const LL_table_s *ll_table) {
    
    alg_list_s* xsolves = alg_list_create(20);
    cube18B_xcross4_s xcross_puzzle = cube18B_xcross4_from_cube18B(&cube);

    //printf("Starting xcross search...\n");
    xcross_search_first_of_each_separate(&xcross_puzzle, xsolves);
    //printf("Finished xcross search: %zu solutions found\n", xsolves->num_algs);
    bool allXsolvesWorked = true;
    for (size_t alg = 0; alg < xsolves->num_algs; alg++) {
        alg_s* xcross_alg = &(xsolves->list[alg]);
        if (!test_xcross_alg(&xcross_puzzle, xcross_alg)) {
            printf("THIS XCROSS ALG DOESNT SOLVE XCROSS\n");
            print_alg(&(xsolves->list[alg]));
            allXsolvesWorked = false;
        }
    } //if (allXsolvesWorked) {
    //    printf("All xsolves worked! :)\n");
    //}

    for (size_t alg = 0; alg < xsolves->num_algs; alg++) {
        alg_s* xcross_alg = &(xsolves->list[alg]);
        cube18B_s new_cube = cube;
        cube18B_apply_alg(&new_cube, xcross_alg);
        
        alg_s *f2l_solve = alg_create(10);
        cube18B_F2L_s F2L_portion = cube18B_F2L_from_cube18B(&new_cube);
        //print_cube18B_F2L(&F2L_portion);
        cube18B_1LLL_s LL_portion = cube18B_1LLL_from_cube18B(&new_cube);
        f2l_stage(F2L_portion, LL_portion, best, xcross_alg, f2l_solve, f2l_table, ll_table, 3);
        //printf("------------------------------------------------\n");
        alg_free(f2l_solve);
    }
    alg_list_free(xsolves);
}

alg_s* solve_cube(cube18B_s cube, const F2L_table_s *f2l_table, const LL_table_s *ll_table) {
    if (!f2l_table || !ll_table) {
        printf("No F2L or last layer table was provided!");
    }
    //printf("F2L entries: %zu\n", F2L_table_entries(f2l_table));
    //printf("LL entries: %zu\n", LL_table_entries(ll_table));

    alg_s* best_solve = NULL;
    //printf("Entering xcross_stage...\n");
    xcross_stage(cube, &best_solve, f2l_table, ll_table);
    //printf("Exited xcross_stage...\n");
    //if (best_solve == NULL) {
    //    printf("best_solve was NULL!!!\n");
    //} else {
    //    printf("best_solve was: \n");
    //    print_alg(best_solve);
    //}
    //printf("clearing xcross_start_ct...\n");
    //xcross4_table_clear(xcross4_start_ct);
    //printf("clearing xcross_end_ct...\n");
    //xcross4_table_clear(xcross4_end_ct);

    return best_solve;
}

uint8_t f2l_pair_orders[24][4] = {
    {0, 1, 2, 3},
    {0, 1, 3, 2},
    {0, 2, 1, 3},
    {0, 2, 3, 1},
    {0, 3, 1, 2},
    {0, 3, 2, 1},
    {1, 0, 2, 3},
    {1, 0, 3, 2},
    {1, 2, 0, 3},
    {1, 2, 3, 0},
    {1, 3, 0, 2},
    {1, 3, 2, 0},
    {2, 0, 1, 3},
    {2, 0, 3, 1},
    {2, 1, 0, 3},
    {2, 1, 3, 0},
    {2, 3, 0, 1},
    {2, 3, 1, 0},
    {3, 0, 1, 2},
    {3, 0, 2, 1},
    {3, 1, 0, 2},
    {3, 1, 2, 0},
    {3, 2, 0, 1},
    {3, 2, 1, 0}
};

LL_table_s* generate_last_layer_table(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t len;
    ssize_t read;

    // generate a cube table twice the size of the number of 1lll states,
    // the number below was chosen arbitrarily 
    LL_table_s *last_layer_table = LL_table_create(131009);

    cube18B_1LLL_s cube;
    alg_s *algorithm = NULL;
    while ((read = getline(&line, &len, file)) != -1) {
        cube = SOLVED_CUBE18B_1LLL;
        alg_s *algorithm = alg_from_alg_str(line);

        // If a line doesn't have a valid algorithm to parse, fail
        if (algorithm == NULL) {
            LL_table_free(last_layer_table);
            last_layer_table = NULL;
            break;
        }

        alg_invert(algorithm);
        cube18B_1LLL_apply_alg(&cube, algorithm);
        alg_invert(algorithm);
        LL_table_insert(last_layer_table, &cube, algorithm);

        alg_free(algorithm);
    }


    free(line);
    fclose(file);
    alg_free(algorithm);
    return last_layer_table;
}

F2L_table_s* generate_f2l_table(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t len;
    ssize_t read;

    // generate a cube table twice the size of the number of 1lll states,
    // the number below was chosen arbitrarily 
    F2L_table_s *f2l_table = F2L_table_create(2909);

    cube18B_F2L_s cube;
    alg_s *algorithm = NULL;

    while ((read = getline(&line, &len, file)) != -1) {
        alg_s *algorithm = alg_from_alg_str(line);

        // If a line doesn't have a valid algorithm to parse, fail
        if (algorithm == NULL) {
            F2L_table_free(f2l_table);
            f2l_table = NULL;
            break;
        }

        for (uint8_t y_turns = 0; y_turns < 4; y_turns++) {
            cube = SOLVED_CUBE18B_F2L;
            alg_rotate_on_y(algorithm, y_turns);
            alg_invert(algorithm);
            cube18B_F2L_apply_alg(&cube, algorithm);
            // a y_turn, as a rotation about U, goes in the opposite direction
            // of the f2l pairs, so to get the f2l_pair an algorithm is associated
            // with, take the negative of y_turns to get that slot
            cube18B_F2L_maskOnPair(&cube, mod4(-y_turns));
            alg_invert(algorithm);
            F2L_table_insert(f2l_table, &cube, algorithm);
            alg_rotate_on_y(algorithm, -y_turns);
        }

        alg_free(algorithm);
    }

    free(line);
    fclose(file);
    alg_free(algorithm);
    return f2l_table;
}
