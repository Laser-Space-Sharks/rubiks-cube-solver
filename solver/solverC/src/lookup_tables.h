#ifndef LOOKUP_TABLES_H
#define LOOKUP_TABLES_H

#include "main.h"

// lookup table for getting the opposite side of a face
static const face_e opposite_faces[NUM_FACES] = {
    FACE_D, FACE_L, FACE_B, FACE_R, FACE_F, FACE_U,
};

static const shift_cube_s cross_mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0x00F00000,
    .state[FACE_F] = 0x00F00000,
    .state[FACE_L] = 0x00F00000,
    .state[FACE_B] = 0x00F00000,
    .state[FACE_D] = 0xF0F0F0F0
};

static const shift_cube_s f2l_1mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFF00000,
    .state[FACE_F] = 0x00FFF000,
    .state[FACE_L] = 0x00F00000,
    .state[FACE_B] = 0x00F00000,
    .state[FACE_D] = 0xF0F0FFF0
};

static const shift_cube_s f2l_2mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFF00000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0x00FFF000,
    .state[FACE_B] = 0x00F00000,
    .state[FACE_D] = 0xF0F0FFFF
};

static const shift_cube_s f2l_3mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFFFF000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0x00FFF000,
    .state[FACE_B] = 0xFFF00000,
    .state[FACE_D] = 0xF0FFFFFF
};

static const shift_cube_s f2l_4mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFFFF000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0xFFFFF000,
    .state[FACE_B] = 0xFFFFF000,
    .state[FACE_D] = 0xFFFFFFFF
};

static const shift_cube_s oll_mask = {
    .state[FACE_U] = 0xFFFFFFFF,
    .state[FACE_R] = 0xFFFFF000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0xFFFFF000,
    .state[FACE_B] = 0xFFFFF000,
    .state[FACE_D] = 0xFFFFFFFF
};

static const shift_cube_s pll_mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0x00000FFF,
    .state[FACE_F] = 0x00000FFF,
    .state[FACE_L] = 0x00000FFF,
    .state[FACE_B] = 0x00000FFF,
    .state[FACE_D] = 0x00000000
};

// Cube piece lookups
static const shift_cube_s corner_piece_mask = {
    .state[FACE_U] = 0xF0F0F0F0,
    .state[FACE_R] = 0xF0F0F0F0,
    .state[FACE_F] = 0xF0F0F0F0,
    .state[FACE_L] = 0xF0F0F0F0,
    .state[FACE_B] = 0xF0F0F0F0,
    .state[FACE_D] = 0xF0F0F0F0
};

static const shift_cube_s edge_piece_mask = {
    .state[FACE_U] = 0x0F0F0F0F,
    .state[FACE_R] = 0x0F0F0F0F,
    .state[FACE_F] = 0x0F0F0F0F,
    .state[FACE_L] = 0x0F0F0F0F,
    .state[FACE_B] = 0x0F0F0F0F,
    .state[FACE_D] = 0x0F0F0F0F
};

static const facelet_pos_s corner_pieces[NUM_CORNERS][3] = {
    {{FACE_U, 0}, {FACE_L, 0}, {FACE_B, 2}},
    {{FACE_U, 2}, {FACE_R, 2}, {FACE_B, 0}},
    {{FACE_U, 4}, {FACE_R, 0}, {FACE_F, 2}},
    {{FACE_U, 6}, {FACE_F, 0}, {FACE_L, 2}},
    {{FACE_D, 0}, {FACE_F, 6}, {FACE_L, 4}},
    {{FACE_D, 2}, {FACE_R, 6}, {FACE_F, 4}},
    {{FACE_D, 4}, {FACE_R, 4}, {FACE_B, 6}},
    {{FACE_D, 6}, {FACE_L, 6}, {FACE_B, 4}},
};

static const facelet_pos_s edge_pieces[NUM_EDGES][2] = {
    {{FACE_U, 1}, {FACE_B, 1}},
    {{FACE_U, 3}, {FACE_R, 1}},
    {{FACE_U, 5}, {FACE_F, 1}},
    {{FACE_U, 7}, {FACE_L, 1}},
    {{FACE_B, 7}, {FACE_R, 3}},
    {{FACE_R, 7}, {FACE_F, 3}},
    {{FACE_F, 7}, {FACE_L, 3}},
    {{FACE_L, 7}, {FACE_B, 3}},
    {{FACE_D, 1}, {FACE_F, 5}},
    {{FACE_D, 3}, {FACE_R, 5}},
    {{FACE_D, 5}, {FACE_B, 5}},
    {{FACE_D, 7}, {FACE_L, 5}},
};

static const face_e f2l_edge_colors[4][2] = {
    {FACE_R, FACE_F},
    {FACE_F, FACE_L},
    {FACE_L, FACE_B},
    {FACE_B, FACE_R}
};

static const face_e f2l_corner_colors[4][3] = {
    {FACE_F, FACE_R, FACE_D},
    {FACE_L, FACE_F, FACE_D},
    {FACE_B, FACE_L, FACE_D},
    {FACE_R, FACE_B, FACE_D}
};

static const face_e rotate_on_y[4][NUM_FACES] = {
    {FACE_U, FACE_R, FACE_F, FACE_L, FACE_B, FACE_D},
    {FACE_U, FACE_B, FACE_R, FACE_F, FACE_L, FACE_D},
    {FACE_U, FACE_L, FACE_B, FACE_R, FACE_F, FACE_D},
    {FACE_U, FACE_F, FACE_L, FACE_B, FACE_R, FACE_D}
};

#define MAX_CUBE_TABLE_DEPTH 7

static const uint32_t cube_table_depth_sizes[MAX_CUBE_TABLE_DEPTH + 1] = {
    1, 31, 487, 6883, 79103, 1201691, 14998309, 175005217
};

#endif // LOOKUP_TABLES_H
