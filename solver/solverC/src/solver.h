#ifndef SOLVER_H
#define SOLVER_H

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
	NUM_FACES
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
 * The full cube with each piece index when unfolded looks like this:
 *     012
 *     7U3
 *     654
 * 012 012 012 012
 * 7L3 7F3 7R3 7B3
 * 654 654 654 654
 *     012
 *     7D3
 *     654
 *
 */

typedef struct {
	uint64_t state[NUM_FACES];
} cube_s;

#endif
