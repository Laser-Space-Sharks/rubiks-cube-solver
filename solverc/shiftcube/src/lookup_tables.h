#ifndef LOOKUP_TABLES_H
#define LOOKUP_TABLES_H

#include "main.h"
#include "shift_cube.h"

static const shift_cube_s cross_mask = {
    .state = {
        0x00000000,
        0x00F00000,
        0x00F00000,
        0x00F00000,
        0x00F00000,
        0xF0F0F0F0
    }
};

static const shift_cube_s f2l_1mask = {
    .state = {
        0x00000000,
        0xFFF00000,
        0x00FFF000,
        0x00F00000,
        0x00F00000,
        0xF0F0FFF0
    }
};

static const shift_cube_s f2l_2mask = {
    .state = {
        0x00000000,
        0xFFF00000,
        0xFFFFF000,
        0x00FFF000,
        0x00F00000,
        0xF0F0FFFF
    }
};

static const shift_cube_s f2l_3mask = {
    .state = {
        0x00000000,
        0xFFFFF000,
        0xFFFFF000,
        0x00FFF000,
        0xFFF00000,
        0xF0FFFFFF
    }
};

static const shift_cube_s f2l_4mask = {
    .state = {
        0x00000000,
        0xFFFFF000,
        0xFFFFF000,
        0xFFFFF000,
        0xFFFFF000,
        0xFFFFFFFF
    }
};

static const shift_cube_s full_cube_mask = {
    .state = {
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    }
};

// Cube piece lookups
static const shift_cube_s corner_piece_mask = {
    .state = {
        0xF0F0F0F0,
        0xF0F0F0F0,
        0xF0F0F0F0,
        0xF0F0F0F0,
        0xF0F0F0F0,
        0xF0F0F0F0
    }
};

static const shift_cube_s edge_piece_mask = {
    .state = {
        0x0F0F0F0F,
        0x0F0F0F0F,
        0x0F0F0F0F,
        0x0F0F0F0F,
        0x0F0F0F0F,
        0x0F0F0F0F
    }
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
