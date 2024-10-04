#include "solver.h"
#include "cube.h"
#include "main.h"
#include "move.h"


cube_s mask_cube(cube_s cube, cube_s mask) {
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        cube.state[face] &= mask.state[face];
    }
    return cube;
}

int stage_recursion(cube_s cube, cube_s mask, move_list_s *moves, uint8_t depth) {
    if (depth == 0) {
        if (compare_cubes(mask_cube(cube, mask), mask_cube(SOLVED_SHIFTCUBE, mask))) {
            // holy crap, we did it!
            return 1;
        }

        // no dice
        return 0;
    }

    // depth > 0
    move_s move = NULL_MOVE;
    move_s prev_move = (moves->length > 1) ? moves->list[moves->length-1] : NULL_MOVE;
    move_s prev_prev_move = (moves->length > 2) ? moves->list[moves->length - 2] : NULL_MOVE;

    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        if (face == prev_move.face) {
            continue;
        }

        if (prev_prev_move.face != FACE_NULL) {
            if (face == opposite_faces[prev_move.face] &&
                (face == prev_prev_move.face || face > prev_move.face)) {
                continue;
            }
        }

        move.face = face;
        for (int8_t turns = 1; turns < 4; turns++) {
            move.turns = turns;
            insert_move(moves, move, moves->length);
            apply_move(&cube, move);
            if (stage_recursion(cube, mask, moves, depth - 1)) {
                // we did it!
                return 1;
            }
            delete_move(moves, moves->length-1);
            move.turns = -turns;
            apply_move(&cube, move); // undo move if it doesn't pan out
        }
    }

    return 0;
}

// Iterative deepening depth-first search
move_list_s solve_stage(cube_s cube, cube_s mask) {
    move_list_s moves;
    init_move_list(&moves, 8);

    for (uint8_t depth = 1; depth <= 20; depth++) {
        if (stage_recursion(cube, mask, &moves, depth)) {
            break;
        }
    }
    return moves;
}

void print_masks() {
    printf("Cross mask:\n");
    print_cube(cross_mask);
    printf("F2L mask:\n");
    print_cube(f2l_1mask);
    printf("OLL mask:\n");
    print_cube(oll_mask);
    printf("PLL mask:\n");
    print_cube(pll_mask);
}
