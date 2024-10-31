#include "solver.h"

#include "cube.h"
#include "cube_table.h"

cube_s masked_cube(const cube_s *cube, const cube_s *mask) {
    cube_s new_cube = *cube;
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        new_cube.state[face] &= mask->state[face];
    }
    return new_cube;
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
