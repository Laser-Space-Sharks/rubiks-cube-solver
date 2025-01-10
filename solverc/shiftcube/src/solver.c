#define _GNU_SOURCE
#include "main.h"

#include "solver.h"

#include "shift_cube.h"
#include "lookup_tables.h"
#include "cube_alg_table.h"

#include <stdio.h>
#include <sys/types.h>


static cube_table_s *xcross_start_ct = NULL;
static cube_table_s *xcross_end_ct   = NULL;

bool init_solver() {
    xcross_start_ct = cube_table_create(cube_table_depth_sizes[5]);
    xcross_end_ct   = cube_table_create(cube_table_depth_sizes[5]);
    if (!xcross_start_ct || !xcross_end_ct) {
        return false;
    }

    return true;
}

void cleanup_solver() {
    cube_table_free(xcross_start_ct);
    cube_table_free(xcross_end_ct);
}

shift_cube_s get_f2l_pair(const shift_cube_s *cube, uint8_t pair) {
    if (pair >= 4) {
        return NULL_CUBE;
    }

    shift_cube_s pair_edge = get_edges(cube, f2l_edge_colors[pair][0], f2l_edge_colors[pair][1]);
    shift_cube_s pair_corner = get_corners(cube, f2l_corner_colors[pair][0], f2l_corner_colors[pair][1], f2l_corner_colors[pair][2]);

    return ored_cube(&pair_edge, &pair_corner);
}

int stage_recursion(shift_cube_s *cube, const shift_cube_s *mask, const shift_cube_s *goal, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        shift_cube_s test = masked_cube(cube, mask);

        if (compare_cubes(&test, goal)) {
            // holy crap, we did it!
            return 1;
        }

        // no dice
        return 0;
    }

    // depth > 0
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
        move_e inv_move = move_inverted[move];
        if (stage_recursion(cube, mask, goal, alg, depth - 1)) {
            // we did it! Begin undoing these cube moves we've accrewed
            apply_move(cube, inv_move);
            return 1;
        }
        // keep going, move didn't pan out
        alg_delete(alg, alg->length-1);
        apply_move(cube, inv_move); // undo move
    }

    return 0;
}


// Iterative deepening depth-first search
alg_s* solve_stage(shift_cube_s cube, shift_cube_s mask) {
    alg_s *alg = alg_create(8);
    shift_cube_s goal = masked_cube(&SOLVED_SHIFTCUBE, &mask);

    for (uint8_t depth = 1; depth <= 10; depth++) {
        if (stage_recursion(&cube, &mask, &goal, alg, depth)) {
            break;
        }
    }

    return alg;
}

int bidirectional_recursion(shift_cube_s *cube, cube_table_s *our_ct, cube_table_s *other_ct, alg_s *alg, uint8_t depth) {
    if (depth == 0) {
        if (!cube_table_lookup(our_ct, cube)) {
            cube_table_insert(our_ct, cube, alg);
        }
        return (cube_table_lookup(other_ct, cube) != NULL);
    }

    if (cube_table_lookup(our_ct, cube) != NULL && 
        cube_table_lookup(our_ct, cube)->list[0].length < alg->length) {
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
        if (bidirectional_recursion(cube, our_ct, other_ct, alg, depth - 1)) {
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

alg_s* bidirectional_search(const shift_cube_s *start, const shift_cube_s *goal, uint8_t max_depth) {
    uint8_t start_depth, end_depth;
    start_depth = end_depth = max_depth/2;

    if (max_depth % 2 == 1) {
        start_depth++;
    }

    if (start_depth > MAX_CUBE_TABLE_DEPTH) {
        printf("Attempted to recurse too deep for cube_table\n");
        return NULL;
    }

    cube_table_s *start_ct = cube_table_create(cube_table_depth_sizes[start_depth]);
    cube_table_s *end_ct   = cube_table_create(cube_table_depth_sizes[end_depth]);

    alg_s *start_alg = alg_create(start_depth);
    alg_s *end_alg   = alg_create(end_depth);

    shift_cube_s start_cube = *start;
    shift_cube_s end_cube   = *goal;

    bool found = false;

    for (uint8_t depth = 0; depth <= start_depth; depth++) {
        if (bidirectional_recursion(&start_cube, start_ct, end_ct, start_alg, depth)) {
            alg_free(end_alg);
            end_alg = alg_copy(&cube_table_lookup(end_ct, &start_cube)->list[0]);
            found = true;
            break;
        }

        if (max_depth % 2 == 1) break;

        if (bidirectional_recursion(&end_cube, end_ct, start_ct, end_alg, depth)) {
            alg_free(start_alg);
            start_alg = alg_copy(&cube_table_lookup(start_ct, &end_cube)->list[0]);
            found = true;
            break;
        }
    } 
    cube_table_free(start_ct);
    cube_table_free(end_ct);
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
    //printf("xcross solution was: "); print_alg(start_alg);
    return start_alg;
}

alg_s* solve_cross(shift_cube_s cube) {
    // match to cross pieces shift_cube_s start_cube = get_edges(&cube, FACE_D, FACE_NULL);
    shift_cube_s goal_cube = get_edges(&SOLVED_SHIFTCUBE, FACE_D, FACE_NULL);

    return bidirectional_search(&cube, &goal_cube, 8);
}

static void last_layer_stage(const shift_cube_s *cube, alg_s **best, const alg_s *xsolve,
                             const alg_s *f2l_solve, const cube_alg_table_s *ll_table) {
    alg_s *solve = alg_copy(xsolve);
    alg_concat(solve, f2l_solve);

    const alg_s *last_layer_alg = cube_alg_table_lookup(ll_table, cube);
    if (last_layer_alg != NULL) {
        alg_concat(solve, last_layer_alg);
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
                      const cube_alg_table_s *ll_table, uint8_t depth) {

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
                         const cube_table_s *f2l_table, const cube_alg_table_s *ll_table) {
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

alg_s* solve_cube(shift_cube_s cube, const cube_table_s *f2l_table, const cube_alg_table_s *ll_table) {
    if (!f2l_table || !ll_table) {
        printf("No F2L or last layer table was provided!");
    }

    alg_s *best_solve = NULL;
    xcross_stage(cube, &best_solve, f2l_table, ll_table);
    cube_table_clear(xcross_start_ct);
    cube_table_clear(xcross_end_ct);

    if (best_solve == NULL) printf("best_solve is NULL!\n");

    return best_solve;
}

cube_alg_table_s* gen_last_layer_table() {
    cube_alg_table_s *ll_table = cube_alg_table_create(131009);
    alg_list_s *ll_algs = alg_list_from_file(LL_PATH);

    for (size_t i = 0; i < ll_algs->num_algs; i++) {
        shift_cube_s cube = SOLVED_SHIFTCUBE;
        alg_invert(&ll_algs->list[i]);
        apply_alg(&cube, &ll_algs->list[i]);
        alg_invert(&ll_algs->list[i]);
        cube_alg_table_overwrite(ll_table, &cube, &ll_algs->list[i]);
    }

    cube_alg_table_overwrite(ll_table, &SOLVED_SHIFTCUBE, &NULL_ALG);

    alg_list_free(ll_algs);
    return ll_table;
}

cube_table_s* gen_f2l_table() {
    cube_table_s *f2l_table = cube_table_create(2909);
    alg_list_s *f2l_algs = alg_list_from_file(F2L_PATH);

    const shift_cube_s solved_f2l_bits = masked_cube(&SOLVED_SHIFTCUBE, &f2l_4mask);
    for (size_t i = 0; i < f2l_algs->num_algs; i++) {
        for (uint8_t y_turns = 0; y_turns < 4; y_turns++) {
            shift_cube_s cube = solved_f2l_bits;
            alg_invert(&f2l_algs->list[i]);
            apply_alg(&cube, &f2l_algs->list[i]);

            // a y_turn, as a rotation about U, goes in the opposite direction
            // of the f2l pairs, so to get the f2l_pair an algorithm is associated
            // with, take the negative of y_turns to get that slot
            cube = get_f2l_pair(&cube, mod4(-y_turns));
            alg_invert(&f2l_algs->list[i]);
            cube_table_insert(f2l_table, &cube, &f2l_algs->list[i]);

            // rotate the algorithm forward for the next iteration
            alg_rotate_on_y(&f2l_algs->list[i], 1);
        }
    }

    alg_list_free(f2l_algs);
    return f2l_table;
}

//cube_table_s* generate_last_layer_table(const char *filename) {
//    FILE *file = fopen(filename, "rb");
//    if (file == NULL) {
//        return NULL;
//    }
//
//    char *line = NULL;
//    size_t len;
//    ssize_t read;
//
//    // generate a cube table twice the size of the number of 1lll states,
//    // the number below was chosen arbitrarily 
//    cube_table_s *last_layer_table = cube_table_create(131009);
//
//    shift_cube_s cube;
//    alg_s *algorithm = NULL;
//    while ((read = getline(&line, &len, file)) != -1) {
//        cube = SOLVED_SHIFTCUBE;
//        alg_s *algorithm = alg_from_alg_str(line);
//
//        // If a line doesn't have a valid algorithm to parse, fail
//        if (algorithm == NULL) {
//            cube_table_free(last_layer_table);
//            last_layer_table = NULL;
//            break;
//        }
//
//        alg_invert(algorithm);
//        apply_alg(&cube, algorithm);
//        alg_invert(algorithm);
//        cube_table_insert(last_layer_table, &cube, algorithm);
//
//        alg_free(algorithm);
//    }
//
//
//    free(line);
//    fclose(file);
//    alg_free(algorithm);
//    return last_layer_table;
//}

//cube_table_s* generate_f2l_table(const char *filename) {
//    FILE *file = fopen(filename, "rb");
//    if (file == NULL) {
//        return NULL;
//    }
//
//    char *line = NULL;
//    size_t len;
//    ssize_t read;
//
//    // generate a cube table twice the size of the number of 1lll states,
//    // the number below was chosen arbitrarily 
//    cube_table_s *f2l_table = cube_table_create(2909);
//
//    shift_cube_s cube;
//    alg_s *algorithm = NULL;
//
//    while ((read = getline(&line, &len, file)) != -1) {
//        shift_cube_s solved_f2l_bits = masked_cube(&SOLVED_SHIFTCUBE, &f2l_4mask);
//        alg_s *algorithm = alg_from_alg_str(line);
//
//        // If a line doesn't have a valid algorithm to parse, fail
//        if (algorithm == NULL) {
//            cube_table_free(f2l_table);
//            f2l_table = NULL;
//            break;
//        }
//
//        for (uint8_t y_turns = 0; y_turns < 4; y_turns++) {
//            cube = solved_f2l_bits;
//            alg_rotate_on_y(algorithm, y_turns);
//            alg_invert(algorithm);
//            apply_alg(&cube, algorithm);
//            // a y_turn, as a rotation about U, goes in the opposite direction
//            // of the f2l pairs, so to get the f2l_pair an algorithm is associated
//            // with, take the negative of y_turns to get that slot
//            cube = get_f2l_pair(&cube, mod4(-y_turns));
//            alg_invert(algorithm);
//            cube_table_insert(f2l_table, &cube, algorithm);
//            alg_rotate_on_y(algorithm, -y_turns);
//        }
//
//        alg_free(algorithm);
//    }
//
//    free(line);
//    fclose(file);
//    alg_free(algorithm);
//    return f2l_table;
//}
