#ifndef SOLVER_H
#define SOLVER_H

#include "main.h"
#include "move.h"

cube_s mask_cube(cube_s cube, cube_s mask);

static const cube_s cross_mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0x00F00000,
    .state[FACE_F] = 0x00F00000,
    .state[FACE_L] = 0x00F00000,
    .state[FACE_B] = 0x00F00000,
    .state[FACE_D] = 0xF0F0F0F0
};

static const cube_s f2l_1mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFF00000,
    .state[FACE_F] = 0x00FFF000,
    .state[FACE_L] = 0x00F00000,
    .state[FACE_B] = 0x00F00000,
    .state[FACE_D] = 0xF0F0FFF0
};

static const cube_s f2l_2mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFF00000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0x00FFF000,
    .state[FACE_B] = 0x00F00000,
    .state[FACE_D] = 0xF0F0FFFF
};

static const cube_s f2l_3mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFFFF000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0x00FFF000,
    .state[FACE_B] = 0xFFF00000,
    .state[FACE_D] = 0xF0FFFFFF
};

static const cube_s f2l_4mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0xFFFFF000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0xFFFFF000,
    .state[FACE_B] = 0xFFFFF000,
    .state[FACE_D] = 0xFFFFFFFF
};

static const cube_s oll_mask = {
    .state[FACE_U] = 0xFFFFFFFF,
    .state[FACE_R] = 0xFFFFF000,
    .state[FACE_F] = 0xFFFFF000,
    .state[FACE_L] = 0xFFFFF000,
    .state[FACE_B] = 0xFFFFF000,
    .state[FACE_D] = 0xFFFFFFFF
};

static const cube_s pll_mask = {
    .state[FACE_U] = 0x00000000,
    .state[FACE_R] = 0x00000FFF,
    .state[FACE_F] = 0x00000FFF,
    .state[FACE_L] = 0x00000FFF,
    .state[FACE_B] = 0x00000FFF,
    .state[FACE_D] = 0x00000000
};

int stage_recursion(cube_s cube, cube_s mask, move_list_s *moves, uint8_t depth);
move_list_s* solve_stage(cube_s cube, cube_s mask);

void print_masks();

#endif // SOLVER_H
