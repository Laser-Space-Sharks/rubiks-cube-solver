#include "solver.h"

#include "cube.h"
#include "cube_table.h"
#include "lookup_tables.h"
#include "move_list.h"

cube_s masked_cube(const cube_s *cube, const cube_s *mask) {
    cube_s new_cube = *cube;
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        new_cube.state[face] &= mask->state[face];
    }
    return new_cube;
}

cube_s get_cross_edges(const cube_s *cube) {
    cube_s cross_cube = NULL_CUBE;
    for (uint8_t edge = 0; edge < NUM_EDGES; edge++) {
        uint8_t facelet_indices[2] = {
            4 * edge_pieces[edge][0].index,
            4 * edge_pieces[edge][1].index
        };

        face_e facelet_colors[2] = {
            (cube->state[edge_pieces[edge][0].face] >> facelet_indices[0]) & 0xF,
            (cube->state[edge_pieces[edge][1].face] >> facelet_indices[1]) & 0xF
        };


        if (facelet_colors[0] == FACE_D || facelet_colors[1] == FACE_D) {
            // clear the bits in the position to set
            cross_cube.state[edge_pieces[edge][0].face] ^= (uint32_t)FACE_NULL << facelet_indices[0];
            cross_cube.state[edge_pieces[edge][1].face] ^= (uint32_t)FACE_NULL << facelet_indices[1];

            // set the bits
            cross_cube.state[edge_pieces[edge][0].face] |= facelet_colors[0] << facelet_indices[0];
            cross_cube.state[edge_pieces[edge][1].face] |= facelet_colors[1] << facelet_indices[1];
        }
    }

    return cross_cube;
}

int stage_recursion(cube_s *cube, const cube_s *mask, const cube_s *goal, cube_table_s *ct, move_list_s *moves, uint8_t depth) {
    if (depth == 0) {
        cube_s test = masked_cube(cube, mask);
        // cube_table_insert(ct, cube, moves);

        if (compare_cubes(&test, goal)) {
            // holy crap, we did it!
            return 1;
        }

        // no dice
        return 0;
    }

    // depth > 0
    move_s move = NULL_MOVE;
    move_s prev_move = (moves->length >= 1) ? moves->list[moves->length-1] : NULL_MOVE;
    move_s prev_prev_move = (moves->length >= 2) ? moves->list[moves->length - 2] : NULL_MOVE;

    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        if (face == prev_move.face) {
            continue;
        }

        if (face == opposite_faces[prev_move.face] &&
            (face == prev_prev_move.face || face > prev_move.face)) {
            continue;
        }

        move.face = face;
        for (int8_t turns = 1; turns < 4; turns++) {
            move.turns = turns;
            move_list_insert(moves, move, moves->length);
            apply_move(cube, move);
            move.turns = -turns;
            if (stage_recursion(cube, mask, goal, ct, moves, depth - 1)) {
                // we did it! Begin undoing these cube moves we've accrewed
                apply_move(cube, move);
                return 1;
            }
            // keep going, move didn't pan out
            move_list_delete(moves, moves->length-1);
            apply_move(cube, move); // undo move
        }
    }

    return 0;
}


// Iterative deepening depth-first search
move_list_s* solve_stage(cube_s cube, cube_s mask) {
    move_list_s *moves = move_list_create(8);
    cube_s goal = masked_cube(&SOLVED_SHIFTCUBE, &mask);

    cube_table_s *ct = cube_table_create(1);

    for (uint8_t depth = 1; depth <= 10; depth++) {
        if (stage_recursion(&cube, &mask, &goal, ct, moves, depth)) {
            break;
        }
    }

//    cube_table_print(ct);
    cube_table_free(ct);

    return moves;
}

int bidirectional_recursion(cube_s *cube, cube_table_s *our_ct, cube_table_s *other_ct, move_list_s *moves, uint8_t depth) {
    if (depth == 0) {
        cube_table_insert(our_ct, cube, moves);

        if (cube_table_lookup(other_ct, cube) != NULL) {
            // holy crap, we did it!
            return 1;
        }

        // no dice
        return 0;
    }

    // depth > 0
    if (cube_table_lookup(our_ct, cube) != NULL) {
        if (cube_table_lookup(our_ct, cube)->length < moves->length) {
            return 0;
        }
    }

    move_s move = NULL_MOVE;
    move_s prev_move = (moves->length >= 1) ? moves->list[moves->length-1] : NULL_MOVE;
    move_s prev_prev_move = (moves->length >= 2) ? moves->list[moves->length - 2] : NULL_MOVE;

    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        if (face == prev_move.face) {
            continue;
        }

        if (face == opposite_faces[prev_move.face] &&
            (face == prev_prev_move.face || face > prev_move.face)) {
            continue;
        }

        move.face = face;
        for (int8_t turns = 1; turns < 4; turns++) {
            move.turns = turns;
            move_list_insert(moves, move, moves->length);
            apply_move(cube, move);
            if (bidirectional_recursion(cube, our_ct, other_ct, moves, depth - 1)) {
                // we did it!
                return 1;
            }

            move.turns = -turns;
            // keep going, move didn't pan out
            move_list_delete(moves, moves->length-1);
            apply_move(cube, move); // undo move
        }
    }

    return 0;
}

move_list_s* solve_cross(cube_s cube) {
    move_list_s *start_moves = move_list_create(MIN_LIST_RESIZE);
    move_list_s *end_moves = move_list_create(MIN_LIST_RESIZE);

    cube_table_s *start_ct = cube_table_create(86837);
    cube_table_s *end_ct = cube_table_create(86837);

    cube_s start_cube = get_cross_edges(&cube);
    cube_s goal_cube = get_cross_edges(&SOLVED_SHIFTCUBE);

    for (uint8_t depth = 0; depth <= 5; depth++) {
        if (bidirectional_recursion(&start_cube, start_ct, end_ct, start_moves, depth)) {
            move_list_free(end_moves);
            end_moves = move_list_copy(cube_table_lookup(end_ct, &start_cube));

            break;
        }

        if (bidirectional_recursion(&goal_cube, end_ct, start_ct, end_moves, depth)) {
            move_list_free(start_moves);
            start_moves = move_list_copy(cube_table_lookup(start_ct, &goal_cube));
            break;
        }
    }

    move_list_invert(end_moves);

    move_list_s *solution = move_list_concat(start_moves, end_moves);

    cube_table_free(start_ct);
    cube_table_free(end_ct);
    move_list_free(start_moves);
    move_list_free(end_moves);

    return solution;
}

move_list_s* solve_1lll(cube_s cube) {
    move_list_s *start_moves = move_list_create(MIN_LIST_RESIZE);
    move_list_s *end_moves = move_list_create(MIN_LIST_RESIZE);

    cube_table_s *start_ct = cube_table_create(217645199);
    cube_table_s *end_ct = cube_table_create(217645199);

    cube_s start_cube = cube;
    cube_s goal_cube = SOLVED_SHIFTCUBE;

    for (uint8_t depth = 0; depth <= 6; depth++) {
        if (bidirectional_recursion(&start_cube, start_ct, end_ct, start_moves, depth)) {
            move_list_free(end_moves);
            end_moves = move_list_copy(cube_table_lookup(end_ct, &start_cube));

            break;
        }

        if (bidirectional_recursion(&goal_cube, end_ct, start_ct, end_moves, depth)) {
            move_list_free(start_moves);
            start_moves = move_list_copy(cube_table_lookup(start_ct, &goal_cube));
            break;
        }
    }

    move_list_invert(end_moves);

    move_list_s *solution = move_list_concat(start_moves, end_moves);

    cube_table_free(start_ct);
    cube_table_free(end_ct);
    move_list_free(start_moves);
    move_list_free(end_moves);

    return solution;
}
