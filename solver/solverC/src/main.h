#ifndef MAIN_H
#define MAIN_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// lookup table for getting the opposite side of a face
static const face_e opposite_faces[NUM_FACES] = {
    FACE_D, FACE_L, FACE_B, FACE_R, FACE_F, FACE_U,
};

#define NUM_EDGES   12
#define NUM_CORNERS  8

typedef struct {
    face_e face;
    uint8_t index;
} facelet_pos_s;

static inline uint8_t mod4(int64_t n) {
    return n & 3;
}

#endif // MAIN_H
