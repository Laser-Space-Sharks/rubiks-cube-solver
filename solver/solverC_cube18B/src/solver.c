#define _GNU_SOURCE

#include "solver.h"

#include "shift_cube.h"
#include "lookup_tables.h"

#include <stdio.h>
#include <sys/types.h>

static xcross_table_s *xcross_start_ct = NULL;
static xcross_table_s *xcross_end_ct   = NULL;

bool init_solver() {
    xcross_start_ct = xcross_table_create(cube_table_depth_sizes[5]);
    xcross_end_ct   = xcross_table_create(cube_table_depth_sizes[5]);
    if (!xcross_start_ct || !xcross_end_ct) {
        return false;
    }

    return true;
}

void cleanup_solver() {
    xcross_table_free(xcross_start_ct);
    xcross_table_free(xcross_end_ct);
}

int bidirectional_recursion(cube18B_xcross_s *cube, xcross_table_s *our_ct, xcross_table_s *other_ct, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        cube18B_xcross_s f2l1 = *cube;
        cube18B_xcross_s f2l2 = *cube;
        cube18B_xcross_s f2l3 = *cube;
        cube18B_xcross_s f2l4 = *cube;
        cube18B_xcross_maskOnPair(&f2l1, 0);
        cube18B_xcross_maskOnPair(&f2l2, 1);
        cube18B_xcross_maskOnPair(&f2l3, 2);
        cube18B_xcross_maskOnPair(&f2l4, 3);
        xcross_table_insert(our_ct, &f2l1, alg);
        xcross_table_insert(our_ct, &f2l2, alg);
        xcross_table_insert(our_ct, &f2l3, alg);
        xcross_table_insert(our_ct, &f2l4, alg);
        xcross_table_insert(our_ct, cube, alg);
        return 0;
    }

    // depth > 0
    if (cube_table_lookup(our_ct, cube) != NULL &&
        cube_table_lookup(our_ct, cube)->list[0].length < alg->length) {
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
        cube18B_xcross_apply_move(cube, move);

        bidirectional_recursion(cube, our_ct, other_ct, alg, depth - 1);
        
        alg_delete(alg, alg->length-1);
        apply_move(cube, move_inverted[move]); // undo move
    }
    return 0;
}

static alg_s* xcross_search(const shift_cube_s *start, const shift_cube_s *goal) {
    if (!xcross_start_ct || !xcross_end_ct) {
        printf("Didn't have the tables initialized :(\n");
        return NULL;
    }

    alg_s *start_alg = alg_create(5);
    alg_s *end_alg   = alg_create(5);

    shift_cube_s start_cube = *start;
    shift_cube_s end_cube   = *goal;
    //printf("=============STARTING XCROSS===========\n");
    //printf("START_CUBE: \n");
    //print_cube_map_colors(start_cube);
    //printf("END_CUBE: \n");
    //print_cube_map_colors(end_cube);

    for (uint8_t depth = 0; depth <= 5; depth++) {
        if (bidirectional_recursion(&start_cube, xcross_start_ct, xcross_end_ct, start_alg, depth)) {
            alg_free(end_alg);
            end_alg = alg_copy(&cube_table_lookup(xcross_end_ct, &start_cube)->list[0]);
            //printf("START ALG WAS: ");
            //print_alg(start_alg);
            //printf("END ALG WAS: ");
            //print_alg(end_alg);
            break;
        }
        if (!compare_cubes(start, &start_cube) || !compare_cubes(goal, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");

        if (bidirectional_recursion(&end_cube, xcross_end_ct, xcross_start_ct, end_alg, depth)) {
            alg_free(start_alg);
            start_alg = alg_copy(&cube_table_lookup(xcross_start_ct, &end_cube)->list[0]);
            //printf("START ALG WAS: ");
            //print_alg(start_alg);
            //printf("END ALG WAS: ");
            //print_alg(end_alg);
            break;
        }
        if (!compare_cubes(start, &start_cube) || !compare_cubes(goal, &end_cube)) printf("BIDIRECTIONAL RECURSION FAILED TO UNDO ITS MOVES\n");
    }

    alg_invert(end_alg);
    alg_concat(start_alg, end_alg);

    alg_free(end_alg);
    return start_alg;
}

alg_s* solve_cross(shift_cube_s cube) {
    // match to cross pieces
    shift_cube_s start_cube = get_edges(&cube, FACE_D, FACE_NULL);
    shift_cube_s goal_cube = get_edges(&SOLVED_SHIFTCUBE, FACE_D, FACE_NULL);

    return bidirectional_search(&start_cube, &goal_cube, 8);
}

static void last_layer_stage(const shift_cube_s *cube, alg_s **best, const alg_s *xsolve,
                             const alg_s *f2l_solve, const cube_table_s *ll_table) {
    alg_s *solve = alg_copy(xsolve);
    alg_concat(solve, f2l_solve);

    const alg_list_s *last_layer_alg = cube_table_lookup(ll_table, cube);
    if (last_layer_alg != NULL) {
        alg_concat(solve, &(last_layer_alg->list[0]));
    }

    alg_simplify(solve);

    if (*best && (*best)->length <= solve->length) {
        alg_free(solve);
        return;
    }

    alg_free(*best);
    *best = solve;
}

static void f2l_stage(shift_cube_s cube, alg_s **best, const alg_s *xsolve,
                      alg_s *f2l_solve, const cube_table_s *f2l_table,
                      const cube_table_s *ll_table, uint8_t depth) {

    const shift_cube_s solved_f2l_bits = masked_cube(&SOLVED_SHIFTCUBE, &f2l_4mask);
    shift_cube_s cube_f2l_bits = masked_cube(&cube, &f2l_4mask);

    // we solved F2L! Proceed to the last layer
    if (compare_cubes(&cube_f2l_bits, &solved_f2l_bits)) {
        last_layer_stage(&cube, best, xsolve, f2l_solve, ll_table);
        return;
    }
    if (depth == 0) printf("5TH PAIR?!\n");

    for (uint8_t pair = 0; pair < 4; pair++) {
        shift_cube_s pair_mask = get_f2l_pair(&cube, pair);
        shift_cube_s solved_pair_mask = get_f2l_pair(&SOLVED_SHIFTCUBE, pair);
        if (compare_cubes(&pair_mask, &solved_pair_mask)) continue;

        const alg_list_s *pair_algs = cube_table_lookup(f2l_table, &pair_mask);
        if (!pair_algs) {
            //printf("THERE ARE NO PAIR ALGS FOR PAIR %hhu\n", pair);
            //print_cube_map_colors(cube);
            //printf("THE XCROSS SOLVE WAS: ");
            //print_alg(xsolve);
            return;
        }

        for (size_t alg = 0; alg < pair_algs->num_algs; alg++) {
            //if (compare_algs(xsolve, "B2 R B' L' F L F") && compare_algs(f2l_solve, "L2 B L U' B' L B2 D B' U B D' B2")) {
            //    print_cube_map_colors(cube);
            //    print_alg(&(pair_algs->list[alg]));
            //}
            shift_cube_s new_cube = cube;
            apply_alg(&new_cube, &pair_algs->list[alg]);
            size_t old_len = f2l_solve->length;
            alg_concat(f2l_solve, &pair_algs->list[alg]);
            f2l_stage(new_cube, best, xsolve, f2l_solve, f2l_table, ll_table, depth-1);
            f2l_solve->length -= f2l_solve->length - old_len;
        }
    }
}

static void xcross_stage(shift_cube_s cube, alg_s **best,
                         const cube_table_s *f2l_table, const cube_table_s *ll_table) {
    shift_cube_s mask_cube   = get_edges(&cube, FACE_D, FACE_NULL);
    shift_cube_s target_cube = get_edges(&SOLVED_SHIFTCUBE, FACE_D, FACE_NULL);

    for (uint8_t pair = 0; pair < 4; pair++) {
        shift_cube_s cube_pair_mask   = get_f2l_pair(&cube, pair);
        shift_cube_s target_pair_mask = get_f2l_pair(&SOLVED_SHIFTCUBE, pair);
        shift_cube_s start_cube       = ored_cube(&mask_cube, &cube_pair_mask);
        shift_cube_s goal_cube        = ored_cube(&target_cube, &target_pair_mask);
        alg_s *xcross_alg       = xcross_search(&start_cube, &goal_cube);

        shift_cube_s new_cube = cube;
        apply_alg(&new_cube, xcross_alg);
        alg_s *f2l_solve = alg_create(10);
        f2l_stage(new_cube, best, xcross_alg, f2l_solve, f2l_table, ll_table, 3);
        alg_free(f2l_solve);
        alg_free(xcross_alg);
    }
}

alg_s* solve_cube(shift_cube_s cube, const cube_table_s *f2l_table, const cube_table_s *ll_table) {
    if (!f2l_table || !ll_table) {
        printf("No F2L or last layer table was provided!");
    }

    alg_s *best_solve = NULL;
    xcross_stage(cube, &best_solve, f2l_table, ll_table);
    cube_table_clear(xcross_start_ct);
    cube_table_clear(xcross_end_ct);

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

cube_table_s* generate_last_layer_table(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t len;
    ssize_t read;

    // generate a cube table twice the size of the number of 1lll states,
    // the number below was chosen arbitrarily 
    cube_table_s *last_layer_table = cube_table_create(131009);

    shift_cube_s cube;
    alg_s *algorithm = NULL;
    while ((read = getline(&line, &len, file)) != -1) {
        cube = SOLVED_SHIFTCUBE;
        alg_s *algorithm = alg_from_alg_str(line);

        // If a line doesn't have a valid algorithm to parse, fail
        if (algorithm == NULL) {
            cube_table_free(last_layer_table);
            last_layer_table = NULL;
            break;
        }

        alg_invert(algorithm);
        apply_alg(&cube, algorithm);
        alg_invert(algorithm);
        cube_table_insert(last_layer_table, &cube, algorithm);

        alg_free(algorithm);
    }


    free(line);
    fclose(file);
    alg_free(algorithm);
    return last_layer_table;
}

cube_table_s* generate_f2l_table(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t len;
    ssize_t read;

    // generate a cube table twice the size of the number of 1lll states,
    // the number below was chosen arbitrarily 
    cube_table_s *f2l_table = cube_table_create(2909);

    shift_cube_s cube;
    alg_s *algorithm = NULL;

    while ((read = getline(&line, &len, file)) != -1) {
        shift_cube_s solved_f2l_bits = masked_cube(&SOLVED_SHIFTCUBE, &f2l_4mask);
        alg_s *algorithm = alg_from_alg_str(line);

        // If a line doesn't have a valid algorithm to parse, fail
        if (algorithm == NULL) {
            cube_table_free(f2l_table);
            f2l_table = NULL;
            break;
        }

        for (uint8_t y_turns = 0; y_turns < 4; y_turns++) {
            cube = solved_f2l_bits;
            alg_rotate_on_y(algorithm, y_turns);
            alg_invert(algorithm);
            apply_alg(&cube, algorithm);
            // a y_turn, as a rotation about U, goes in the opposite direction
            // of the f2l pairs, so to get the f2l_pair an algorithm is associated
            // with, take the negative of y_turns to get that slot
            cube = get_f2l_pair(&cube, mod4(-y_turns));
            alg_invert(algorithm);
            cube_table_insert(f2l_table, &cube, algorithm);
            alg_rotate_on_y(algorithm, -y_turns);
        }

        alg_free(algorithm);
    }

    free(line);
    fclose(file);
    alg_free(algorithm);
    return f2l_table;
}
