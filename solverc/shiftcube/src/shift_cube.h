#ifndef CUBE_H
#define CUBE_H

#include <stdbool.h>

#include "main.h"
#include "solver_print.h"

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

typedef struct {
    uint32_t state[NUM_FACES];
} shift_cube_s;

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

// fast rol instruction that simplifies to rolq on x86 and rol on arm64, used
// to quickly rotate cube faces
static inline uint32_t rolq(uint32_t n, uint8_t c) {
    const uint32_t mask = CHAR_BIT*sizeof(n) - 1;

    c &= mask;
    return (n << c) | (n >> (-c & mask));
}

shift_cube_s* cube_copy(const shift_cube_s *cube);
shift_cube_s ored_cube(const shift_cube_s *a, const shift_cube_s *b);
shift_cube_s masked_cube(const shift_cube_s *cube, const shift_cube_s *mask);
bool compare_cubes(const shift_cube_s *a, const shift_cube_s *b);

shift_cube_s get_edges(const shift_cube_s *cube, face_e color1, face_e color2);
shift_cube_s get_corners(const shift_cube_s *cube, face_e color1, face_e color2, face_e color3);

void init_move_bitrolls();
void old_apply_move(shift_cube_s *c, move_s m);
void apply_move(shift_cube_s *c, move_e m);
void apply_alg(shift_cube_s *cube, const alg_s *alg);

void print_face(uint32_t face);

void print_cube_map(shift_cube_s cube);
void print_cube_line(shift_cube_s cube);
void print_cube_map_colors(shift_cube_s cube);
void print_cube_line_colors(shift_cube_s cube);

#endif // CUBE_H
