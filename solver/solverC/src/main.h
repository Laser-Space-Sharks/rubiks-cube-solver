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
 * with each byte being a different piece, indexed in the following way:
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

typedef struct {
    uint64_t state[NUM_FACES];
} cube_s;

// fast rol instruction that simplifies to rolq (on x86 and rol on arm64)
static inline uint64_t rolq(uint64_t n, uint8_t c) {
    const uint64_t mask = CHAR_BIT*sizeof(n) - 1;

    c &= mask;
    return (n << c) | (n >> (-c & mask));
}

// takes positive mod of integer parameter with unsigned divisor
static inline uint64_t positive_mod(int64_t n, uint64_t m) {
    return (n % m + m) % m;
}

static const cube_s SOLVED_SHIFTCUBE = {
    .state = {
        (uint64_t)FACE_U        | (uint64_t)FACE_U <<  8 | (uint64_t)FACE_U << 16 |
        (uint64_t)FACE_U <<  56 |                          (uint64_t)FACE_U << 24 |
        (uint64_t)FACE_U <<  48 | (uint64_t)FACE_U << 40 | (uint64_t)FACE_U << 32,

        (uint64_t)FACE_R        | (uint64_t)FACE_R <<  8 | (uint64_t)FACE_R << 16 |
        (uint64_t)FACE_R <<  56 |                          (uint64_t)FACE_R << 24 |
        (uint64_t)FACE_R <<  48 | (uint64_t)FACE_R << 40 | (uint64_t)FACE_R << 32,

        (uint64_t)FACE_F        | (uint64_t)FACE_F <<  8 | (uint64_t)FACE_F << 16 |
        (uint64_t)FACE_F <<  56 |                          (uint64_t)FACE_F << 24 |
        (uint64_t)FACE_F <<  48 | (uint64_t)FACE_F << 40 | (uint64_t)FACE_F << 32,

        (uint64_t)FACE_L        | (uint64_t)FACE_L <<  8 | (uint64_t)FACE_L << 16 |
        (uint64_t)FACE_L <<  56 |                          (uint64_t)FACE_L << 24 |
        (uint64_t)FACE_L <<  48 | (uint64_t)FACE_L << 40 | (uint64_t)FACE_L << 32,

        (uint64_t)FACE_B        | (uint64_t)FACE_B <<  8 | (uint64_t)FACE_B << 16 |
        (uint64_t)FACE_B <<  56 |                          (uint64_t)FACE_B << 24 |
        (uint64_t)FACE_B <<  48 | (uint64_t)FACE_B << 40 | (uint64_t)FACE_B << 32,

        (uint64_t)FACE_D        | (uint64_t)FACE_D <<  8 | (uint64_t)FACE_D << 16 |
        (uint64_t)FACE_D <<  56 |                          (uint64_t)FACE_D << 24 |
        (uint64_t)FACE_D <<  48 | (uint64_t)FACE_D << 40 | (uint64_t)FACE_D << 32,
    }
};

#endif // MAIN_H