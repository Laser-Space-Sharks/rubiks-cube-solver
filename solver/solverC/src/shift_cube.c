#include "shift_cube.h"

#include "lookup_tables.h"

// lookup table for the faces that connect to a given face
static const face_e side_faces[NUM_FACES][NUM_SIDES] = {
    {FACE_B, FACE_R, FACE_F, FACE_L}, // FACE_U 
    {FACE_U, FACE_B, FACE_D, FACE_F}, // FACE_R 
    {FACE_U, FACE_R, FACE_D, FACE_L}, // FACE_F
    {FACE_U, FACE_F, FACE_D, FACE_B}, // FACE_L
    {FACE_U, FACE_L, FACE_D, FACE_R}, // FACE_B
    {FACE_F, FACE_R, FACE_B, FACE_L}  // FACE_D
};

// bitmasks for masking a given side of a face
static const uint32_t side_masks[NUM_SIDES] = {
    0x00000FFF, // SIDE_U
    0x000FFF00, // SIDE_R
    0x0FFF0000, // SIDE_D
    0xFF00000F  // SIDE_L
};

// lookup table for getting masks to the side pieces that a given face "sees"
static const uint32_t move_side_masks[NUM_FACES][NUM_SIDES] = {
    {side_masks[SIDE_U], side_masks[SIDE_U], side_masks[SIDE_U], side_masks[SIDE_U]}, // FACE_U
    {side_masks[SIDE_R], side_masks[SIDE_L], side_masks[SIDE_R], side_masks[SIDE_R]}, // FACE_R
    {side_masks[SIDE_D], side_masks[SIDE_L], side_masks[SIDE_U], side_masks[SIDE_R]}, // FACE F
    {side_masks[SIDE_L], side_masks[SIDE_L], side_masks[SIDE_L], side_masks[SIDE_R]}, // FACE L
    {side_masks[SIDE_U], side_masks[SIDE_L], side_masks[SIDE_D], side_masks[SIDE_R]}, // FACE B
    {side_masks[SIDE_D], side_masks[SIDE_D], side_masks[SIDE_D], side_masks[SIDE_D]}  // FACE D
};

// lookup table for getting which side a given face sees in a given direction
// e.g. the SIDE_U bits of FACE_F 'see' the bottom SIDE_D bits of FACE_U
static const side_e move_sfaces[NUM_FACES][NUM_SIDES] = {
    {SIDE_U, SIDE_U, SIDE_U, SIDE_U}, // FACE_U
    {SIDE_R, SIDE_L, SIDE_R, SIDE_R}, // FACE_R
    {SIDE_D, SIDE_L, SIDE_U, SIDE_R}, // FACE F
    {SIDE_L, SIDE_L, SIDE_L, SIDE_R}, // FACE L
    {SIDE_U, SIDE_L, SIDE_D, SIDE_R}, // FACE B
    {SIDE_D, SIDE_D, SIDE_D, SIDE_D}  // FACE D
};

shift_cube_s* cube_copy(const shift_cube_s *cube) {
    if (cube == NULL) {
        return NULL;
    }

    shift_cube_s *copy = (shift_cube_s*)malloc(sizeof(shift_cube_s));
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy->state, cube->state, 24);

    return copy;
}

shift_cube_s ored_cube(const shift_cube_s *a, const shift_cube_s *b) {
    shift_cube_s new_cube = *a;

    new_cube.state[FACE_U] |= b->state[FACE_U];
    new_cube.state[FACE_R] |= b->state[FACE_R];
    new_cube.state[FACE_F] |= b->state[FACE_F];
    new_cube.state[FACE_L] |= b->state[FACE_L];
    new_cube.state[FACE_B] |= b->state[FACE_B];
    new_cube.state[FACE_D] |= b->state[FACE_D];

    return new_cube;
}

shift_cube_s masked_cube(const shift_cube_s *cube, const shift_cube_s *mask) {
    shift_cube_s new_cube = *cube;

    new_cube.state[FACE_U] &= mask->state[FACE_U];
    new_cube.state[FACE_R] &= mask->state[FACE_R];
    new_cube.state[FACE_F] &= mask->state[FACE_F];
    new_cube.state[FACE_L] &= mask->state[FACE_L];
    new_cube.state[FACE_B] &= mask->state[FACE_B];
    new_cube.state[FACE_D] &= mask->state[FACE_D];

    return new_cube;
}

// test for the equality of two cube states
bool compare_cubes(const shift_cube_s *a, const shift_cube_s *b) {
    if (memcmp(a->state, b->state, 24) == 0) {
        return true;
    }

    return false;
}

// builds a masked cube with edges that match color1 and color2,
// if color1 or color2 are FACE_NULL, it matches to any face which matches
// the criterion set by another color
shift_cube_s get_edges(const shift_cube_s *cube, face_e color1, face_e color2) {
    shift_cube_s masked_edges = NULL_CUBE;
    for (uint8_t edge = 0; edge < NUM_EDGES; edge++) {
        uint8_t facelet_indices[2] = {
            4 * edge_pieces[edge][0].index,
            4 * edge_pieces[edge][1].index
        };

        face_e facelet_colors[2] = {
            (cube->state[edge_pieces[edge][0].face] >> facelet_indices[0]) & 0xF,
            (cube->state[edge_pieces[edge][1].face] >> facelet_indices[1]) & 0xF
        };

        if ((color1 == FACE_NULL || facelet_colors[0] == color1 || facelet_colors[1] == color1) &&
            (color2 == FACE_NULL || facelet_colors[0] == color2 || facelet_colors[1] == color2)) {
            // set the bits
            masked_edges.state[edge_pieces[edge][0].face] |= facelet_colors[0] << facelet_indices[0];
            masked_edges.state[edge_pieces[edge][1].face] |= facelet_colors[1] << facelet_indices[1];

            if (color1 != FACE_NULL && color2 != FACE_NULL) {
                break;
            }
        }

    }

    return masked_edges;
}

// builds a masked cube with edges that match color1, color2, and color3,
// if color1 or color2 or color3 are FACE_NULL, it matches to any face which matches
// the criterion set by another color
shift_cube_s get_corners(const shift_cube_s *cube, face_e color1, face_e color2, face_e color3) {
    shift_cube_s masked_corners = NULL_CUBE;
    for (uint8_t corner = 0; corner < NUM_CORNERS; corner++) {
        uint8_t facelet_indices[3] = {
            4 * corner_pieces[corner][0].index,
            4 * corner_pieces[corner][1].index,
            4 * corner_pieces[corner][2].index
        };

        face_e facelet_colors[3] = {
            (cube->state[corner_pieces[corner][0].face] >> facelet_indices[0]) & 0xF,
            (cube->state[corner_pieces[corner][1].face] >> facelet_indices[1]) & 0xF,
            (cube->state[corner_pieces[corner][2].face] >> facelet_indices[2]) & 0xF
        };

        if ((color1 == FACE_NULL || facelet_colors[0] == color1 || facelet_colors[1] == color1 || facelet_colors[2] == color1) &&
            (color2 == FACE_NULL || facelet_colors[0] == color2 || facelet_colors[1] == color2 || facelet_colors[2] == color2) &&
            (color3 == FACE_NULL || facelet_colors[0] == color3 || facelet_colors[1] == color3 || facelet_colors[2] == color3)) {

            // set the bits
            masked_corners.state[corner_pieces[corner][0].face] |= facelet_colors[0] << facelet_indices[0];
            masked_corners.state[corner_pieces[corner][1].face] |= facelet_colors[1] << facelet_indices[1];
            masked_corners.state[corner_pieces[corner][2].face] |= facelet_colors[2] << facelet_indices[2];

            if (color1 != FACE_NULL && color2 != FACE_NULL && color3 != FACE_NULL) {
                break;
            }
        }
    }

    return masked_corners;
}

// lookup table generated by init_move_bitrolls that, given a face, a side of
// that face, and a count of turns of that face (modulo 4), returns the amount
// by which the bits of a side face need to be rolled in order to align them
// for insertion into the appropriate target side face
static uint8_t move_bitrolls[NUM_FACES][NUM_SIDES][4];

static uint8_t new_move_bitrolls[NUM_SIDES][NUM_SIDES];

// a lookup table that given a side of a face and a number of turns of said
// face (modulo 4) returns the 'target side' of the input side, i.e. which side
// the facelets of the input side would end up after turning a face
static const side_e target_sides[NUM_SIDES][4] = {
    {SIDE_U, SIDE_R, SIDE_D, SIDE_L},
    {SIDE_R, SIDE_D, SIDE_L, SIDE_U},
    {SIDE_D, SIDE_L, SIDE_U, SIDE_R},
    {SIDE_L, SIDE_U, SIDE_R, SIDE_D}
};

void init_move_bitrolls() {
    for (face_e f = FACE_U; f < NUM_FACES; f++) {
    for (side_e s = SIDE_U; s < NUM_SIDES; s++) {
    for (uint8_t turns = 0; turns < 4; turns++) {
        move_bitrolls[f][s][turns] = mod4(move_sfaces[f][target_sides[s][turns]]
                                          - move_sfaces[f][s]) * 8;
    }}}
}

void init_new_move_bitrolls() {
    for (side_e s1 = SIDE_U; s1 < NUM_SIDES; s1++) {
        for (side_e s2 = SIDE_U; s2 < NUM_SIDES; s2++) {
            new_move_bitrolls[s1][s2] = (s2 - s1) * 8;
        }
    }
}

// Apply a move to the cube using bit manipulations
void apply_move(shift_cube_s *c, move_s m) {
    // make the turn count positive
    m.turns = mod4(m.turns);

    /* these are some handy macros for quickly indexing into the tables used
     * for applying a move, where the input s is a side of m.face
     */
    #define move_sface(s)       side_faces[m.face][s]
    #define target_sface(s)     side_faces[m.face][target_sides[s][m.turns]]
    #define side_roll(s)        move_bitrolls[m.face][s][m.turns]

    uint32_t sfacelets[NUM_SIDES] = {
        c->state[move_sface(SIDE_U)] & move_side_masks[m.face][SIDE_U],
        c->state[move_sface(SIDE_R)] & move_side_masks[m.face][SIDE_R],
        c->state[move_sface(SIDE_D)] & move_side_masks[m.face][SIDE_D],
        c->state[move_sface(SIDE_L)] & move_side_masks[m.face][SIDE_L]
    };

    // rotate the face to be turned
    c->state[m.face] = rolq(c->state[m.face], 8*m.turns);

    // at this point, we need to turn the pieces on the adjacent (side) faces
    // of the face we just rotated, and propogate those rotations forward
    c->state[move_sface(SIDE_U)] &= ~move_side_masks[m.face][SIDE_U];
    c->state[move_sface(SIDE_R)] &= ~move_side_masks[m.face][SIDE_R];
    c->state[move_sface(SIDE_D)] &= ~move_side_masks[m.face][SIDE_D];
    c->state[move_sface(SIDE_L)] &= ~move_side_masks[m.face][SIDE_L];

    c->state[target_sface(SIDE_U)] |= rolq(sfacelets[SIDE_U], side_roll(SIDE_U));
    c->state[target_sface(SIDE_R)] |= rolq(sfacelets[SIDE_R], side_roll(SIDE_R));
    c->state[target_sface(SIDE_D)] |= rolq(sfacelets[SIDE_D], side_roll(SIDE_D));
    c->state[target_sface(SIDE_L)] |= rolq(sfacelets[SIDE_L], side_roll(SIDE_L));
}

static inline void swap_sides(uint32_t *face1_bits, uint32_t *face2_bits,
                              face_e face1, face_e face2,
                              side_e face1_s, side_e face2_s) {
    *face1_bits = rolq(*face1_bits, new_move_bitrolls[face1_s][face2_s]);
    *face1_bits ^= *face2_bits & side_masks[face2_s];
    *face2_bits ^= *face1_bits & side_masks[face2_s];
    *face1_bits ^= *face2_bits & side_masks[face2_s];
    *face1_bits = rolq(*face1_bits, -new_move_bitrolls[face1_s][face2_s]);
}

void new_apply_move(shift_cube_s *c, move_e m) {
    switch (m) {
        case MOVE_U:
            c->state[FACE_U] = rolq(c->state[FACE_U], 8);
            swap_sides(&(c->state[FACE_B]), &(c->state[FACE_R]),
                       FACE_B, FACE_R, SIDE_U, SIDE_U);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_B]),
                       FACE_L, FACE_B, SIDE_U, SIDE_U);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_L]),
                       FACE_F, FACE_L, SIDE_U, SIDE_U);
            break;
        case MOVE_U2:
            c->state[FACE_U] = rolq(c->state[FACE_U], 16);
            swap_sides(&(c->state[FACE_B]), &(c->state[FACE_F]),
                       FACE_B, FACE_F, SIDE_U, SIDE_U);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_R]),
                       FACE_L, FACE_R, SIDE_U, SIDE_U);
            break;
        case MOVE_UPRIME:
            c->state[FACE_U] = rolq(c->state[FACE_U], 24);
            swap_sides(&(c->state[FACE_R]), &(c->state[FACE_B]),
                       FACE_R, FACE_B, SIDE_U, SIDE_U);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_R]),
                       FACE_F, FACE_R, SIDE_U, SIDE_U);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_F]),
                       FACE_L, FACE_F, SIDE_U, SIDE_U);
            break;
        case MOVE_R:
            c->state[FACE_R] = rolq(c->state[FACE_R], 8);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_B]),
                       FACE_U, FACE_B, SIDE_R, SIDE_L);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_U]),
                       FACE_F, FACE_U, SIDE_R, SIDE_R);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_F]),
                       FACE_D, FACE_F, SIDE_R, SIDE_R);
            break;
        case MOVE_R2:
            c->state[FACE_R] = rolq(c->state[FACE_R], 16);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_D]),
                       FACE_U, FACE_D, SIDE_R, SIDE_R);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_B]),
                       FACE_F, FACE_B, SIDE_R, SIDE_L);
            break;
        case MOVE_RPRIME:
            c->state[FACE_R] = rolq(c->state[FACE_R], 24);
            swap_sides(&(c->state[FACE_B]), &(c->state[FACE_D]),
                       FACE_B, FACE_D, SIDE_L, SIDE_R);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_F]),
                       FACE_D, FACE_F, SIDE_R, SIDE_R);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_U]),
                       FACE_F, FACE_U, SIDE_R, SIDE_R);
            break;
        case MOVE_F:
            c->state[FACE_F] = rolq(c->state[FACE_F], 8);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_R]),
                       FACE_U, FACE_R, SIDE_D, SIDE_L);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_U]),
                       FACE_L, FACE_U, SIDE_R, SIDE_D);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_L]),
                       FACE_D, FACE_L, SIDE_U, SIDE_R);
            break;
        case MOVE_F2:
            c->state[FACE_F] = rolq(c->state[FACE_F], 16);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_D]),
                       FACE_U, FACE_D, SIDE_D, SIDE_U);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_R]),
                       FACE_L, FACE_R, SIDE_R, SIDE_L);
            break;
        case MOVE_FPRIME:
            c->state[FACE_F] = rolq(c->state[FACE_F], 24);
            swap_sides(&(c->state[FACE_R]), &(c->state[FACE_D]),
                       FACE_R, FACE_D, SIDE_L, SIDE_U);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_L]),
                       FACE_D, FACE_L, SIDE_U, SIDE_R);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_U]),
                       FACE_L, FACE_U, SIDE_R, SIDE_D);
            break;
        case MOVE_L:
            c->state[FACE_L] = rolq(c->state[FACE_L], 8);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_F]),
                       FACE_D, FACE_F, SIDE_L, SIDE_L);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_U]),
                       FACE_U, FACE_F, SIDE_L, SIDE_L);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_B]),
                       FACE_U, FACE_B, SIDE_L, SIDE_R);
            break;
        case MOVE_L2:
            c->state[FACE_L] = rolq(c->state[FACE_L], 16);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_D]),
                       FACE_U, FACE_D, SIDE_L, SIDE_L);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_B]),
                       FACE_F, FACE_B, SIDE_L, SIDE_R);
            break;
        case MOVE_LPRIME:
            c->state[FACE_L] = rolq(c->state[FACE_L], 24);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_F]),
                       FACE_U, FACE_F, SIDE_L, SIDE_L);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_D]),
                       FACE_F, FACE_D, SIDE_L, SIDE_L);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_B]),
                       FACE_D, FACE_B, SIDE_L, SIDE_R);
            break;
        case MOVE_B:
            c->state[FACE_B] = rolq(c->state[FACE_B], 8);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_U]),
                       FACE_L, FACE_U, SIDE_L, SIDE_U);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_R]),
                       FACE_R, FACE_U, SIDE_U, SIDE_R);
            swap_sides(&(c->state[FACE_R]), &(c->state[FACE_D]),
                       FACE_R, FACE_D, SIDE_R, SIDE_D);
            break;
        case MOVE_B2:
            c->state[FACE_B] = rolq(c->state[FACE_B], 16);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_D]),
                       FACE_U, FACE_D, SIDE_U, SIDE_D);
            swap_sides(&(c->state[FACE_R]), &(c->state[FACE_L]),
                       FACE_R, FACE_L, SIDE_R, SIDE_L);
            break;
        case MOVE_BPRIME:
            c->state[FACE_B] = rolq(c->state[FACE_B], 24);
            swap_sides(&(c->state[FACE_U]), &(c->state[FACE_L]),
                       FACE_U, FACE_L, SIDE_U, SIDE_L);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_D]),
                       FACE_L, FACE_D, SIDE_L, SIDE_D);
            swap_sides(&(c->state[FACE_D]), &(c->state[FACE_R]),
                       FACE_D, FACE_R, SIDE_D, SIDE_R);

            break;
        case MOVE_D:
            c->state[FACE_D] = rolq(c->state[FACE_D], 8);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_B]),
                       FACE_L, FACE_B, SIDE_D, SIDE_D);
            swap_sides(&(c->state[FACE_B]), &(c->state[FACE_R]),
                       FACE_B, FACE_R, SIDE_D, SIDE_D);
            swap_sides(&(c->state[FACE_R]), &(c->state[FACE_F]),
                       FACE_R, FACE_F, SIDE_D, SIDE_D);
            break;
        case MOVE_D2:
            c->state[FACE_D] = rolq(c->state[FACE_D], 16);
            swap_sides(&(c->state[FACE_B]), &(c->state[FACE_F]),
                       FACE_B, FACE_F, SIDE_D, SIDE_D);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_R]),
                       FACE_L, FACE_R, SIDE_D, SIDE_D);
            break;
        case MOVE_DPRIME:
            c->state[FACE_D] = rolq(c->state[FACE_D], 24);
            swap_sides(&(c->state[FACE_B]), &(c->state[FACE_R]),
                       FACE_B, FACE_R, SIDE_D, SIDE_D);
            swap_sides(&(c->state[FACE_L]), &(c->state[FACE_B]),
                       FACE_L, FACE_B, SIDE_D, SIDE_D);
            swap_sides(&(c->state[FACE_F]), &(c->state[FACE_L]),
                       FACE_F, FACE_L, SIDE_D, SIDE_D);
            break;
    }

//    switch (m.turns) {
//        case 1:
//            // U <-> R => L <-> U => D <-> L
//            swap_sides(&(c->state[side_faces[m.face][SIDE_U]]), &(c->state[side_faces[m.face][SIDE_R]]),
//                       side_faces[m.face][SIDE_U], side_faces[m.face][SIDE_R],
//                       move_sfaces[m.face][SIDE_U], move_sfaces[m.face][SIDE_R]);
//            swap_sides(&(c->state[side_faces[m.face][SIDE_L]]), &(c->state[side_faces[m.face][SIDE_U]]),
//                       side_faces[m.face][SIDE_L], side_faces[m.face][SIDE_U],
//                       move_sfaces[m.face][SIDE_L], move_sfaces[m.face][SIDE_U]);
//            swap_sides(&(c->state[side_faces[m.face][SIDE_D]]), &(c->state[side_faces[m.face][SIDE_L]]),
//                       side_faces[m.face][SIDE_D], side_faces[m.face][SIDE_L],
//                       move_sfaces[m.face][SIDE_D], move_sfaces[m.face][SIDE_L]);
//            break;
//        case 2:
//            // U <-> D => R <-> L
//            swap_sides(&(c->state[side_faces[m.face][SIDE_U]]), &(c->state[side_faces[m.face][SIDE_D]]),
//                       side_faces[m.face][SIDE_U], side_faces[m.face][SIDE_D],
//                       move_sfaces[m.face][SIDE_U], move_sfaces[m.face][SIDE_D]);
//            swap_sides(&(c->state[side_faces[m.face][SIDE_R]]), &(c->state[side_faces[m.face][SIDE_L]]),
//                       side_faces[m.face][SIDE_R], side_faces[m.face][SIDE_L],
//                       move_sfaces[m.face][SIDE_R], move_sfaces[m.face][SIDE_L]);
//            break;
//        case 3:
//            // R <-> U => D <-> R => L <-> D
//            swap_sides(&(c->state[side_faces[m.face][SIDE_R]]), &(c->state[side_faces[m.face][SIDE_U]]),
//                       side_faces[m.face][SIDE_R], side_faces[m.face][SIDE_U],
//                       move_sfaces[m.face][SIDE_R], move_sfaces[m.face][SIDE_U]);
//            swap_sides(&(c->state[side_faces[m.face][SIDE_D]]), &(c->state[side_faces[m.face][SIDE_R]]),
//                       side_faces[m.face][SIDE_D], side_faces[m.face][SIDE_R],
//                       move_sfaces[m.face][SIDE_D], move_sfaces[m.face][SIDE_R]);
//            swap_sides(&(c->state[side_faces[m.face][SIDE_L]]), &(c->state[side_faces[m.face][SIDE_D]]),
//                       side_faces[m.face][SIDE_L], side_faces[m.face][SIDE_D],
//                       move_sfaces[m.face][SIDE_L], move_sfaces[m.face][SIDE_D]);
//            break;
//    }
}

// Apply all the moves from a move_list on a cube
void apply_alg(shift_cube_s *cube, const alg_s *alg) {
    for (size_t i = 0; i < alg->length; i++) {
        apply_move(cube, alg->moves[i]);
    }
}

void print_face(uint32_t face_bits) {
    printf("%c%c%c\n%c %c\n%c%c%c\n", 
           get_piece(face_bits, 0), get_piece(face_bits, 1), get_piece(face_bits, 2),
           get_piece(face_bits, 7),                          get_piece(face_bits, 3),
           get_piece(face_bits, 6), get_piece(face_bits, 5), get_piece(face_bits, 4));
}

void print_cube_map(shift_cube_s cube) {
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

void print_cube_map_colors(shift_cube_s cube) {
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

void print_cube_line(shift_cube_s cube) {
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        for (uint8_t idx = 0; idx < 8; idx++) {
            printf("%c", get_piece(cube.state[face], idx));
        }
        printf(" ");
    }
}

void print_cube_line_colors(shift_cube_s cube) {
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        for (uint8_t idx = 0; idx < 8; idx++) {
            print_piece(cube.state[face], idx);
        }
        printf(" ");
    }
}
