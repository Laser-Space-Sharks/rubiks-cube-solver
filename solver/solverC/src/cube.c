#include "cube.h"

cube_s* cube_copy(const cube_s *cube) {
    if (cube == NULL) {
        return NULL;
    }

    cube_s *copy = (cube_s*)malloc(sizeof(cube_s));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *cube;

    return copy;
}

// test for the equality of two cube states
int compare_cubes(cube_s a, cube_s b) {
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        if (a.state[face] != b.state[face]) {
            return 0;
        }
    }
    return 1;
}

// Apply a move to the cube using bitshifts
void apply_move(cube_s *cube, move_s move) {
    // make the turn count positive 
    uint8_t turns_pos = positive_mod(move.turns, NUM_SIDES);

    // copy current side faces for the loop
    uint32_t og_sfaces[NUM_SIDES] = {
        cube->state[side_faces[move.face][SIDE_U]],
        cube->state[side_faces[move.face][SIDE_R]],
        cube->state[side_faces[move.face][SIDE_D]],
        cube->state[side_faces[move.face][SIDE_L]]
    };

    // rotate the face to be turned
    cube->state[move.face] = rolq(cube->state[move.face], 8 * turns_pos);

    // rotate the side pieces
    for (side_e side = SIDE_U; side < NUM_SIDES; side++) {
        side_e target_side  = (side + turns_pos) % NUM_SIDES;
        face_e target_sface = side_faces[move.face][target_side];
        uint8_t side_turns = positive_mod(turn_sides_table[move.face][target_side] -
                                          turn_sides_table[move.face][side], NUM_SIDES);

        // clear the bits of the side pieces
        cube->state[target_sface] &= ~turn_mask_table[move.face][target_side];
        // set the bits of the side pieces 
        cube->state[target_sface] |=  turn_mask_table[move.face][target_side] & 
                                      rolq(og_sfaces[side], side_turns*8);
	}
}

// Apply all the moves from a move_list on a cube
void apply_move_list(cube_s *cube, const move_list_s *moves) {
    for (size_t i = 0; i < moves->length; i++) {
        apply_move(cube, moves->list[i]);
    }
}

void print_face(uint32_t face_bits) {
    printf("%c%c%c\n%c %c\n%c%c%c\n", 
           get_piece(face_bits, 0), get_piece(face_bits, 1), get_piece(face_bits, 2),
           get_piece(face_bits, 7),                          get_piece(face_bits, 3),
           get_piece(face_bits, 6), get_piece(face_bits, 5), get_piece(face_bits, 4));
}

void print_cube(cube_s cube) {
    printf("    %c%c%c\n    %c%c%c\n    %c%c%c\n",
           get_piece(cube.state[FACE_U], 0), get_piece(cube.state[FACE_U], 1), get_piece(cube.state[FACE_U], 2),
           get_piece(cube.state[FACE_U], 7), CHAR_U,                           get_piece(cube.state[FACE_U], 3),
           get_piece(cube.state[FACE_U], 6), get_piece(cube.state[FACE_U], 5), get_piece(cube.state[FACE_U], 4));
    printf("%c%c%c %c%c%c %c%c%c %c%c%c\n",
           get_piece(cube.state[FACE_L], 0), get_piece(cube.state[FACE_L], 1), get_piece(cube.state[FACE_L], 2),
           get_piece(cube.state[FACE_F], 0), get_piece(cube.state[FACE_F], 1), get_piece(cube.state[FACE_F], 2),
           get_piece(cube.state[FACE_R], 0), get_piece(cube.state[FACE_R], 1), get_piece(cube.state[FACE_R], 2),
           get_piece(cube.state[FACE_B], 0), get_piece(cube.state[FACE_B], 1), get_piece(cube.state[FACE_B], 2));
    printf("%c%c%c %c%c%c %c%c%c %c%c%c\n",
           get_piece(cube.state[FACE_L], 7), CHAR_L,  get_piece(cube.state[FACE_L], 3),
           get_piece(cube.state[FACE_F], 7), CHAR_F,  get_piece(cube.state[FACE_F], 3),
           get_piece(cube.state[FACE_R], 7), CHAR_R,  get_piece(cube.state[FACE_R], 3),
           get_piece(cube.state[FACE_B], 7), CHAR_B,  get_piece(cube.state[FACE_B], 3));
    printf("%c%c%c %c%c%c %c%c%c %c%c%c\n",
           get_piece(cube.state[FACE_L], 6), get_piece(cube.state[FACE_L], 5), get_piece(cube.state[FACE_L], 4),
           get_piece(cube.state[FACE_F], 6), get_piece(cube.state[FACE_F], 5), get_piece(cube.state[FACE_F], 4),
           get_piece(cube.state[FACE_R], 6), get_piece(cube.state[FACE_R], 5), get_piece(cube.state[FACE_R], 4),
           get_piece(cube.state[FACE_B], 6), get_piece(cube.state[FACE_B], 5), get_piece(cube.state[FACE_B], 4));
    printf("    %c%c%c\n    %c%c%c\n    %c%c%c\n\n",
           get_piece(cube.state[FACE_D], 0), get_piece(cube.state[FACE_D], 1), get_piece(cube.state[FACE_D], 2),
           get_piece(cube.state[FACE_D], 7), CHAR_D,                           get_piece(cube.state[FACE_D], 3),
           get_piece(cube.state[FACE_D], 6), get_piece(cube.state[FACE_D], 5), get_piece(cube.state[FACE_D], 4));
}

void print_cube_colors(cube_s cube) {
    printf("    "); print_piece(cube.state[FACE_U], 0); print_piece(cube.state[FACE_U], 1); print_piece(cube.state[FACE_U], 2); printf("\n");
    printf("    "); print_piece(cube.state[FACE_U], 7); print_piece(FACE_U, 0);             print_piece(cube.state[FACE_U], 3); printf("\n");
    printf("    "); print_piece(cube.state[FACE_U], 6); print_piece(cube.state[FACE_U], 5); print_piece(cube.state[FACE_U], 4); printf("\n");

    print_piece(cube.state[FACE_L], 0); print_piece(cube.state[FACE_L], 1); print_piece(cube.state[FACE_L], 2); printf(" ");
    print_piece(cube.state[FACE_F], 0); print_piece(cube.state[FACE_F], 1); print_piece(cube.state[FACE_F], 2); printf(" ");
    print_piece(cube.state[FACE_R], 0); print_piece(cube.state[FACE_R], 1); print_piece(cube.state[FACE_R], 2); printf(" ");
    print_piece(cube.state[FACE_B], 0); print_piece(cube.state[FACE_B], 1); print_piece(cube.state[FACE_B], 2); printf("\n");

    print_piece(cube.state[FACE_L], 7); print_piece(FACE_L, 0); print_piece(cube.state[FACE_L], 3); printf(" ");
    print_piece(cube.state[FACE_F], 7); print_piece(FACE_F, 0); print_piece(cube.state[FACE_F], 3); printf(" ");
    print_piece(cube.state[FACE_R], 7); print_piece(FACE_R, 0); print_piece(cube.state[FACE_R], 3); printf(" ");
    print_piece(cube.state[FACE_B], 7); print_piece(FACE_B, 0); print_piece(cube.state[FACE_B], 3); printf("\n");

    print_piece(cube.state[FACE_L], 6); print_piece(cube.state[FACE_L], 5); print_piece(cube.state[FACE_L], 4); printf(" ");
    print_piece(cube.state[FACE_F], 6); print_piece(cube.state[FACE_F], 5); print_piece(cube.state[FACE_F], 4); printf(" ");
    print_piece(cube.state[FACE_R], 6); print_piece(cube.state[FACE_R], 5); print_piece(cube.state[FACE_R], 4); printf(" ");
    print_piece(cube.state[FACE_B], 6); print_piece(cube.state[FACE_B], 5); print_piece(cube.state[FACE_B], 4); printf("\n");

    printf("    "); print_piece(cube.state[FACE_D], 0); print_piece(cube.state[FACE_D], 1); print_piece(cube.state[FACE_D], 2); printf("\n");
    printf("    "); print_piece(cube.state[FACE_D], 7); print_piece(FACE_D, 0);             print_piece(cube.state[FACE_D], 3); printf("\n");
    printf("    "); print_piece(cube.state[FACE_D], 6); print_piece(cube.state[FACE_D], 5); print_piece(cube.state[FACE_D], 4); printf("\n");
}
