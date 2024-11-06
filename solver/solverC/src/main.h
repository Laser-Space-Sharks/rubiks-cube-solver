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
    NUM_FACES,
    FACE_NULL
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
} cube_s;

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

static const cube_s SOLVED_SHIFTCUBE = {
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

static const cube_s NULL_CUBE = {
    .state = {
        (uint32_t)FACE_NULL        | (uint32_t)FACE_NULL <<  4 | (uint32_t)FACE_NULL <<  8 |
        (uint32_t)FACE_NULL <<  28 |                             (uint32_t)FACE_NULL << 12 |
        (uint32_t)FACE_NULL <<  24 | (uint32_t)FACE_NULL << 20 | (uint32_t)FACE_NULL << 16,

        (uint32_t)FACE_NULL        | (uint32_t)FACE_NULL <<  4 | (uint32_t)FACE_NULL <<  8 |
        (uint32_t)FACE_NULL <<  28 |                             (uint32_t)FACE_NULL << 12 |
        (uint32_t)FACE_NULL <<  24 | (uint32_t)FACE_NULL << 20 | (uint32_t)FACE_NULL << 16,

        (uint32_t)FACE_NULL        | (uint32_t)FACE_NULL <<  4 | (uint32_t)FACE_NULL <<  8 |
        (uint32_t)FACE_NULL <<  28 |                             (uint32_t)FACE_NULL << 12 |
        (uint32_t)FACE_NULL <<  24 | (uint32_t)FACE_NULL << 20 | (uint32_t)FACE_NULL << 16,

        (uint32_t)FACE_NULL        | (uint32_t)FACE_NULL <<  4 | (uint32_t)FACE_NULL <<  8 |
        (uint32_t)FACE_NULL <<  28 |                             (uint32_t)FACE_NULL << 12 |
        (uint32_t)FACE_NULL <<  24 | (uint32_t)FACE_NULL << 20 | (uint32_t)FACE_NULL << 16,

        (uint32_t)FACE_NULL        | (uint32_t)FACE_NULL <<  4 | (uint32_t)FACE_NULL <<  8 |
        (uint32_t)FACE_NULL <<  28 |                             (uint32_t)FACE_NULL << 12 |
        (uint32_t)FACE_NULL <<  24 | (uint32_t)FACE_NULL << 20 | (uint32_t)FACE_NULL << 16,

        (uint32_t)FACE_NULL        | (uint32_t)FACE_NULL <<  4 | (uint32_t)FACE_NULL <<  8 |
        (uint32_t)FACE_NULL <<  28 |                             (uint32_t)FACE_NULL << 12 |
        (uint32_t)FACE_NULL <<  24 | (uint32_t)FACE_NULL << 20 | (uint32_t)FACE_NULL << 16,
    }
};

static const move_s NULL_MOVE = {
    .face = FACE_NULL,
    .turns = 0
};

#endif // MAIN_H
