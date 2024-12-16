#ifndef MAIN_H
#define MAIN_H

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef enum : uint8_t {
    FACE_U = 0,
    FACE_R = 1,
    FACE_F = 2,
    FACE_L = 3,
    FACE_B = 4,
    FACE_D = 5,
    NUM_FACES=6,
    FACE_NULL=6
} face_e;

typedef enum : char {
    CHAR_U = 'U',
    CHAR_R = 'R',
    CHAR_F = 'F',
    CHAR_L = 'L',
    CHAR_B = 'B',
    CHAR_D = 'D'
} char_e;

typedef enum : uint8_t {
    SIDE_U = 0,
    SIDE_R = 1,
    SIDE_D = 2,
    SIDE_L = 3,
    NUM_SIDES
} side_e;

const char* movePrints[NUM_MOVES] = {
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

typedef enum {
    MOVE_U = 0,
    MOVE_U2 = 1,
    MOVE_U3 = 2,
    MOVE_R = 3,
    MOVE_R2 = 4,
    MOVE_R3 = 5,
    MOVE_F = 6,
    MOVE_F2 = 7,
    MOVE_F3 = 8,
    MOVE_L = 9,
    MOVE_L2 = 10,
    MOVE_L3 = 11,
    MOVE_B = 12,
    MOVE_B2 = 13,
    MOVE_B3 = 14,
    MOVE_D = 15,
    MOVE_D2 = 16,
    MOVE_D3 = 17,
    MOVE_NULL = 18,
    NUM_MOVES = 18
} move_e;

const move_e inverted_moves[NUM_MOVES+1] = {
    MOVE_U3, MOVE_U2, MOVE_U,
    MOVE_R3, MOVE_R2, MOVE_R,
    MOVE_F3, MOVE_F2, MOVE_F,
    MOVE_L3, MOVE_L2, MOVE_L,
    MOVE_B3, MOVE_B2, MOVE_B,
    MOVE_D3, MOVE_D2, MOVE_D,
    MOVE_NULL
};
const face_e faces_moves[NUM_MOVES+1] = {
    FACE_U, FACE_U, FACE_U,
    FACE_R, FACE_R, FACE_R,
    FACE_F, FACE_F, FACE_F,
    FACE_L, FACE_L, FACE_L,
    FACE_B, FACE_B, FACE_B,
    FACE_D, FACE_D, FACE_D,
    FACE_NULL
};
const face_e opposite_faces_moves[NUM_MOVES+1] = {
    FACE_D, FACE_D, FACE_D,
    FACE_L, FACE_L, FACE_L,
    FACE_B, FACE_B, FACE_B,
    FACE_R, FACE_R, FACE_R,
    FACE_F, FACE_F, FACE_F,
    FACE_U, FACE_U, FACE_U,
    FACE_NULL
};
const move_s move_definitions[NUM_MOVES+1] = {
    {FACE_U, 1},
    {FACE_U, 2},
    {FACE_U, 3},
    {FACE_R, 1},
    {FACE_R, 2},
    {FACE_R, 3},
    {FACE_F, 1},
    {FACE_F, 2},
    {FACE_F, 3},
    {FACE_L, 1},
    {FACE_L, 2},
    {FACE_L, 3},
    {FACE_B, 1},
    {FACE_B, 2},
    {FACE_B, 3},
    {FACE_D, 1},
    {FACE_D, 2},
    {FACE_D, 3},
    {FACE_NULL, 0},
};
const move_e add_related_moves[NUM_MOVES][NUM_MOVES] = {
//       U          U2         U3        R          R2          R3         F         F2         F3         L          L2         L3         B          B2         B3         D          D2         D3
    {MOVE_U2,   MOVE_U3,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_U3,   MOVE_NULL, MOVE_U,    MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_U,    MOVE_U2,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_R2,   MOVE_R3,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_R3,   MOVE_NULL, MOVE_R,    MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_R,    MOVE_R2,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_F2,   MOVE_F3,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_F3,   MOVE_NULL, MOVE_F,    MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_F,    MOVE_F2,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_L2,   MOVE_L3,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_L3,   MOVE_NULL, MOVE_L,    MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_L,    MOVE_L2,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_B2,   MOVE_B3,   MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_B3,   MOVE_NULL, MOVE_B,    MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_B,    MOVE_B2,   MOVE_NULL, MOVE_NULL, MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_D2,   MOVE_D3,   MOVE_NULL},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_D3,   MOVE_NULL, MOVE_D},
    {MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_NULL, MOVE_D,    MOVE_D2},
};
const move_e moves_rotate_on_y[4][NUM_MOVES] = {
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3},
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3},
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3},
    {MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3}
};

typedef enum : uint8_t {
    CUBIE_UR  = 0,
    CUBIE_RU  = 1,
    CUBIE_UF  = 2,
    CUBIE_FU  = 3,
    CUBIE_UL  = 4,
    CUBIE_LU  = 5,
    CUBIE_UB  = 6,
    CUBIE_BU  = 7,
    CUBIE_RF  = 8,
    CUBIE_FR  = 9,
    CUBIE_RB  = 10,
    CUBIE_BR  = 11,
    CUBIE_RD  = 12,
    CUBIE_DR  = 13,
    CUBIE_FL  = 14,
    CUBIE_LF  = 15,
    CUBIE_FD  = 16,
    CUBIE_DF  = 17,
    CUBIE_LB  = 18,
    CUBIE_BL  = 19,
    CUBIE_LD  = 20,
    CUBIE_DL  = 21,
    CUBIE_BD  = 22,
    CUBIE_DB  = 23,
    CUBIE_FUR = 24,
    CUBIE_URF = 25,
    CUBIE_RFU = 26,
    CUBIE_LUF = 27,
    CUBIE_UFL = 28,
    CUBIE_FLU = 29,
    CUBIE_BUL = 30,
    CUBIE_ULB = 31,
    CUBIE_LBU = 32,
    CUBIE_RUB = 33,
    CUBIE_UBR = 34,
    CUBIE_BRU = 35,
    CUBIE_FRD = 36,
    CUBIE_DFR = 37,
    CUBIE_RDF = 38,
    CUBIE_LFD = 39,
    CUBIE_DLF = 40,
    CUBIE_FDL = 41,
    CUBIE_BLD = 42,
    CUBIE_DBL = 43,
    CUBIE_LDB = 44,
    CUBIE_RBD = 45,
    CUBIE_DRB = 46,
    CUBIE_BDR = 47, // This is the last real cubie. All cubies beyond are kept for colorSequence encoding
    CUBIE_FRU = 48,
    CUBIE_UFR = 49,
    CUBIE_RUF = 50,
    CUBIE_LFU = 51,
    CUBIE_ULF = 52,
    CUBIE_FUL = 53,
    CUBIE_BLU = 54,
    CUBIE_UBL = 55,
    CUBIE_LUB = 56,
    CUBIE_RBU = 57,
    CUBIE_URB = 58,
    CUBIE_BUR = 59,
    CUBIE_FDR = 60,
    CUBIE_DRF = 61,
    CUBIE_RFD = 62,
    CUBIE_LDF = 63,
    CUBIE_DFL = 64,
    CUBIE_FLD = 65,
    CUBIE_BDL = 66,
    CUBIE_DLB = 67,
    CUBIE_LBD = 68,
    CUBIE_RDB = 69,
    CUBIE_DBR = 70,
    CUBIE_BRD = 71,
    NUM_CUBIES = 48,
    NUM_SEQUENCES = 72,
    CUBIE_NULL = 72
} cubie_e;

/* Each element of a cube_s is a bitfield representing the state of a cube face,
 * with each 4-bit-word being a different piece, indexed in the following way:
 *
 * 012
 * 7 3
 * 654
 *
 * The full cube when unfolded looks like this:
 *     UUU
 *     UUU
 *     UUU
 * LLL FFF RRR BBB
 * LLL FFF RRR BBB
 * LLL FFF RRR BBB
 *     DDD
 *     DDD
 *     DDD
 *
 */
#define NUM_EDGES   12
#define NUM_CORNERS  8

typedef struct {
    uint32_t state[NUM_FACES];
} shift_cube_s;

typedef struct {
    cubie_e cubies[18];
} cube18B_s;
typedef struct {
    cubie_e cubies[12];
} cube18B_xcross_s;
typedef struct {
    cubie_e cubies[6];
} cube18B_1LLL_s;

typedef struct {
    face_e face;
    int8_t turns;
} move_s;

typedef struct {
    face_e face;
    uint8_t index;
} facelet_pos_s;

// fast rol instruction that simplifies to rolq on x86 and rol on arm64
static inline uint32_t rolq(uint32_t n, uint8_t c) {
    const uint32_t mask = CHAR_BIT*sizeof(n) - 1;

    c &= mask;
    return (n << c) | (n >> (-c & mask));
}

// takes positive mod of integer parameter with unsigned divisor
static inline uint32_t positive_mod(int64_t n, uint64_t m) {
    return (n % m + m) % m;
}

static inline uint8_t mod4(int64_t n) {
    return n & 3;
}

static const shift_cube_s SOLVED_SHIFTCUBE = {
    .state = {
        (uint32_t)FACE_U        | (uint32_t)FACE_U <<  4 | (uint32_t)FACE_U <<  8 |
        (uint32_t)FACE_U <<  28 |                          (uint32_t)FACE_U << 12 |
        (uint32_t)FACE_U <<  24 | (uint32_t)FACE_U << 20 | (uint32_t)FACE_U << 16,

        (uint32_t)FACE_R        | (uint32_t)FACE_R <<  4 | (uint32_t)FACE_R <<  8 |
        (uint32_t)FACE_R <<  28 |                          (uint32_t)FACE_R << 12 |
        (uint32_t)FACE_R <<  24 | (uint32_t)FACE_R << 20 | (uint32_t)FACE_R << 16,

        (uint32_t)FACE_F        | (uint32_t)FACE_F <<  4 | (uint32_t)FACE_F <<  8 |
        (uint32_t)FACE_F <<  28 |                          (uint32_t)FACE_F << 12 |
        (uint32_t)FACE_F <<  24 | (uint32_t)FACE_F << 20 | (uint32_t)FACE_F << 16,

        (uint32_t)FACE_L        | (uint32_t)FACE_L <<  4 | (uint32_t)FACE_L <<  8 |
        (uint32_t)FACE_L <<  28 |                          (uint32_t)FACE_L << 12 |
        (uint32_t)FACE_L <<  24 | (uint32_t)FACE_L << 20 | (uint32_t)FACE_L << 16,

        (uint32_t)FACE_B        | (uint32_t)FACE_B <<  4 | (uint32_t)FACE_B <<  8 |
        (uint32_t)FACE_B <<  28 |                          (uint32_t)FACE_B << 12 |
        (uint32_t)FACE_B <<  24 | (uint32_t)FACE_B << 20 | (uint32_t)FACE_B << 16,

        (uint32_t)FACE_D        | (uint32_t)FACE_D <<  4 | (uint32_t)FACE_D <<  8 |
        (uint32_t)FACE_D <<  28 |                          (uint32_t)FACE_D << 12 |
        (uint32_t)FACE_D <<  24 | (uint32_t)FACE_D << 20 | (uint32_t)FACE_D << 16,
    }
};

static const cube18B_s SOLVED_CUBE18B = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB
    }
};
static const cube18B_xcross_s SOLVED_CUBE18B_XCROSS = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD
    }
};
static const cube18B_1LLL_s SOLVED_CUBE18B_1LLL = {
    .cubies = {
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB
    }
};

static const shift_cube_s NULL_CUBE = {
    .state = {
        (uint32_t)0,
        (uint32_t)0,
        (uint32_t)0,
        (uint32_t)0,
        (uint32_t)0,
        (uint32_t)0
    }
};

static const move_s NULL_MOVE = {
    .face = FACE_NULL,
    .turns = 0
};

#endif // MAIN_H
