#include "solver.h"
#include "cube.h"

const cube_s cross_mask = {
    .state[FACE_U] = 0x0000000000000000,
    .state[FACE_R] = 0x0000FF0000000000,
    .state[FACE_F] = 0x0000FF0000000000,
    .state[FACE_L] = 0x0000FF0000000000,
    .state[FACE_B] = 0x0000FF0000000000,
    .state[FACE_D] = 0xFF00FF00FF00FF00
};

const cube_s f2l_mask = {
    .state[FACE_U] = 0x0000000000000000,
    .state[FACE_R] = 0xFFFF00FFFF000000,
    .state[FACE_F] = 0xFFFF00FFFF000000,
    .state[FACE_L] = 0xFFFF00FFFF000000,
    .state[FACE_B] = 0xFFFF00FFFF000000,
    .state[FACE_D] = 0x00FF00FF00FF00FF
};

const cube_s oll_mask = {
    .state[FACE_U] = 0xFFFFFFFFFFFFFFFF,
    .state[FACE_R] = 0x0000000000FFFFFF,
    .state[FACE_F] = 0x0000000000FFFFFF,
    .state[FACE_L] = 0x0000000000FFFFFF,
    .state[FACE_B] = 0x0000000000FFFFFF,
    .state[FACE_D] = 0x0000000000000000
};

const cube_s pll_mask = {
    .state[FACE_U] = 0x0000000000000000,
    .state[FACE_R] = 0x0000000000FFFFFF,
    .state[FACE_F] = 0x0000000000FFFFFF,
    .state[FACE_L] = 0x0000000000FFFFFF,
    .state[FACE_B] = 0x0000000000FFFFFF,
    .state[FACE_D] = 0x0000000000000000
};

void print_masks() {
    printf("Cross mask:\n");
    print_cube(cross_mask);
    printf("F2L mask:\n");
    print_cube(f2l_mask);
    printf("OLL mask:\n");
    print_cube(oll_mask);
    printf("PLL mask:\n");
    print_cube(pll_mask);
}