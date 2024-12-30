#ifndef MOVE_H
#define MOVE_H

#include "main.h"

typedef enum : uint8_t {
    MOVE_U  = 0,
    MOVE_U2 = 1,
    MOVE_U3 = 2,
    MOVE_R  = 3,
    MOVE_R2 = 4,
    MOVE_R3 = 5,
    MOVE_F  = 6,
    MOVE_F2 = 7,
    MOVE_F3 = 8,
    MOVE_L  = 9, 
    MOVE_L2 = 10,
    MOVE_L3 = 11,
    MOVE_B  = 12,
    MOVE_B2 = 13,
    MOVE_B3 = 14,
    MOVE_D  = 15,
    MOVE_D2 = 16,
    MOVE_D3 = 17,
    MOVE_NULL = 18,
    NUM_MOVES = 18
} move_e;

typedef struct {
    uint8_t face : 4;
    uint8_t turns : 4;
} move_s;

static const move_s NULL_MOVE = {
    .face = FACE_NULL,
    .turns = 0
};


static const move_e move_base[NUM_MOVES+1] = {
    MOVE_U, MOVE_U, MOVE_U,
    MOVE_R, MOVE_R, MOVE_R,
    MOVE_F, MOVE_F, MOVE_F,
    MOVE_L, MOVE_L, MOVE_L,
    MOVE_B, MOVE_B, MOVE_B,
    MOVE_D, MOVE_D, MOVE_D,
};

static const uint8_t move_turns[NUM_MOVES+1] = {
    1, 2, 3,
    1, 2, 3,
    1, 2, 3,
    1, 2, 3,
    1, 2, 3,
    1, 2, 3,
    0
};

static const face_e move_faces[NUM_MOVES+1] = {
    FACE_U, FACE_U, FACE_U,
    FACE_R, FACE_R, FACE_R,
    FACE_F, FACE_F, FACE_F,
    FACE_L, FACE_L, FACE_L,
    FACE_B, FACE_B, FACE_B,
    FACE_D, FACE_D, FACE_D,
    FACE_NULL
};

static const move_e move_inverted[NUM_MOVES+1] = {
    MOVE_U3, MOVE_U2, MOVE_U,
    MOVE_R3, MOVE_R2, MOVE_R,
    MOVE_F3, MOVE_F2, MOVE_F,
    MOVE_L3, MOVE_L2, MOVE_L,
    MOVE_B3, MOVE_B2, MOVE_B,
    MOVE_D3, MOVE_D2, MOVE_D,
    MOVE_NULL
};

static const move_e move_rotated_on_y[4][NUM_MOVES] = {
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3},
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3},
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3},
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3}
};

static const char* movePrints[NUM_MOVES] = {
    "U",
    "U2",
    "U'",
    "R",
    "R2",
    "R'",
    "F",
    "F2",
    "F'",
    "L",
    "L2",
    "L'",
    "B",
    "B2",
    "B'",
    "D",
    "D2",
    "D'"
};

static const move_s move_as_struct[NUM_MOVES+1] = {
    (move_s){FACE_U, 1}, (move_s){FACE_U, 2}, (move_s){FACE_U, 3},
    (move_s){FACE_R, 1}, (move_s){FACE_R, 2}, (move_s){FACE_R, 3},
    (move_s){FACE_F, 1}, (move_s){FACE_F, 2}, (move_s){FACE_F, 3},
    (move_s){FACE_L, 1}, (move_s){FACE_L, 2}, (move_s){FACE_L, 3},
    (move_s){FACE_B, 1}, (move_s){FACE_B, 2}, (move_s){FACE_B, 3},
    (move_s){FACE_D, 1}, (move_s){FACE_D, 2}, (move_s){FACE_D, 3},
    NULL_MOVE
};

//static move_e move_as_enum(move_s move_as_struct) {
//    return (move_e)((move_as_struct.face * 3) + (mod4(move_as_struct.turns) - 1));
//}

//static move_s move_s_add(move_s m1, move_s m2) {
//    if (m1.face != m2.face) {
//        return NULL_MOVE;
//    }
//
//    return (move_s) {
//        m1.face,
//        mod4(m1.turns + m2.turns)
//    };
//}

static inline move_e move_e_add(move_e m1, move_e m2) {
    uint8_t turns = mod4(move_turns[m1] + move_turns[m2]);
    if (move_faces[m1] != move_faces[m2] || !turns) {
        return MOVE_NULL;
    }

    return (move_e)(move_base[m1] + turns-1);
}

typedef struct {
    move_s move1;
    move_s move2;
} MovePair;

static inline bool MovePair_is_singleMove(MovePair pair) {
    return (pair.move2.face == NULL_MOVE.face && pair.move2.turns == NULL_MOVE.turns);
}

#endif // MOVE_H
