#ifndef MAIN_H
#define MAIN_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* LL_PATH = "../../ALGORITHMS/FULL_1LLL_ALGORITHMS.txt";
static const char* F2L_PATH = "../../ALGORITHMS/FULL_F2L_ALGORITHMS.txt";
static const char* INTER_MOVE_TABLE_PATH = "../../servoCoding/ServoOptimizationTable.txt";
static const char* INTER_MOVE_TABLE_RSS_PATH = "../../servoCoding/ServoOptimizationTable_rootpaths.txt";

typedef enum face : uint8_t {
    FACE_U = 0,
    FACE_R = 1,
    FACE_F = 2,
    FACE_L = 3,
    FACE_B = 4,
    FACE_D = 5,
    NUM_FACES=6,
    FACE_NULL=6
} face_e;

static inline face_e face_from_char(char c) {
    switch (c) {
        case 'U':
            return FACE_U;
        case 'R':
            return FACE_R;
        case 'F':
            return FACE_F;
        case 'L':
            return FACE_L;
        case 'B':
            return FACE_B;
        case 'D':
            return FACE_D;
        default:
            return FACE_NULL;
    }
}

static const char facePrints[7] = {'U', 'R', 'F', 'L', 'B', 'D', 'N'};

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

// lookup table for getting the opposite side of a face
static const face_e opposite_faces[NUM_FACES+1] = {
    FACE_D, FACE_L, FACE_B, FACE_R, FACE_F, FACE_U, FACE_NULL
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

static bool unordered_match_faces_2x2(face_e a1, face_e a2, face_e b1, face_e b2) {
    return ((a1 == b1 || a1 == b2) &&
            (a2 == b1 || a2 == b2));
}
static bool unordered_match_faces_3x3(face_e a1, face_e a2, face_e a3, face_e b1, face_e b2, face_e b3) {
    return ((a1 == b1 || a1 == b2 || a1 == b3) &&
            (a2 == b1 || a2 == b2 || a2 == b3) &&
            (a3 == b1 || a3 == b2 || a3 == b3));
}

static int8_t find_face_in_2(face_e a1, face_e a2, face_e b) {
    uint8_t ind;
    if (b == a1) ind = 0;
    else if (b == a2) ind = 1;
    else ind = -1;
    return ind;
}

static int8_t find_face_in_3(face_e a1, face_e a2, face_e a3, face_e b) {
    uint8_t ind;
    if (b == a1) ind = 0;
    else if (b == a2) ind = 1;
    else if (b == a3) ind = 2;
    else ind = -1;
    return ind;
}

#endif // MAIN_H
