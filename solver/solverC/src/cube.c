#include "cube.h"
#include "solver.h"

// lookup table for the faces that connect to a given face
const face_e side_faces[NUM_FACES][NUM_SIDES] = {
	{FACE_B, FACE_R, FACE_F, FACE_L}, // FACE_U 
	{FACE_U, FACE_B, FACE_D, FACE_F}, // FACE_R 
	{FACE_U, FACE_R, FACE_D, FACE_L}, // FACE_F
	{FACE_U, FACE_F, FACE_D, FACE_B}, // FACE_L
	{FACE_U, FACE_L, FACE_D, FACE_R}, // FACE_B
	{FACE_F, FACE_R, FACE_B, FACE_L}  // FACE_D
};

// bitmasks for masking a given side of a face
const uint64_t side_masks[NUM_SIDES] = {
	0x0000000000FFFFFF, // SIDE_U
	0x000000FFFFFF0000, // SIDE_R
	0x00FFFFFF00000000, // SIDE_D
	0xFFFF0000000000FF  // SIDE_L
};

// lookup table for getting masks to the side pieces that a given face "sees"
const uint64_t turn_mask_table[NUM_FACES][NUM_SIDES] = {
	{side_masks[SIDE_U], side_masks[SIDE_U], side_masks[SIDE_U], side_masks[SIDE_U]}, // FACE_U
	{side_masks[SIDE_R], side_masks[SIDE_L], side_masks[SIDE_R], side_masks[SIDE_R]}, // FACE_R
	{side_masks[SIDE_D], side_masks[SIDE_L], side_masks[SIDE_U], side_masks[SIDE_R]}, // FACE F
	{side_masks[SIDE_L], side_masks[SIDE_L], side_masks[SIDE_L], side_masks[SIDE_R]}, // FACE L
	{side_masks[SIDE_U], side_masks[SIDE_L], side_masks[SIDE_D], side_masks[SIDE_R]}, // FACE B
	{side_masks[SIDE_D], side_masks[SIDE_D], side_masks[SIDE_D], side_masks[SIDE_D]}  // FACE D
};

// lookup table for getting sides in a given face turn
const side_e turn_sides_table[NUM_FACES][NUM_SIDES] = {
	{SIDE_U, SIDE_U, SIDE_U, SIDE_U}, // FACE_U
	{SIDE_R, SIDE_L, SIDE_R, SIDE_R}, // FACE_R
	{SIDE_D, SIDE_L, SIDE_U, SIDE_R}, // FACE F
	{SIDE_L, SIDE_L, SIDE_L, SIDE_R}, // FACE L
	{SIDE_U, SIDE_L, SIDE_D, SIDE_R}, // FACE B
	{SIDE_D, SIDE_D, SIDE_D, SIDE_D}  // FACE D
};

// Apply a move to the cube using bitshifts
void apply_move(cube_s *cube, move_s move) {
	// make the turn count positive 
	uint8_t turns_pos = positive_mod(move.turns, NUM_SIDES);

	// copy current side faces for the loop
	uint64_t og_sfaces[NUM_SIDES] = {
		cube->state[side_faces[move.face][SIDE_U]],
		cube->state[side_faces[move.face][SIDE_R]],
		cube->state[side_faces[move.face][SIDE_D]],
		cube->state[side_faces[move.face][SIDE_L]]
	};

	// rotate the face to be turned
	cube->state[move.face] = rolq(cube->state[move.face], 16 * turns_pos);

	// rotate the side pieces
	for (side_e side = SIDE_U; side < NUM_SIDES; side++) {
		side_e target_side  = (side + turns_pos) % NUM_SIDES;
		face_e target_sface = side_faces[move.face][target_side];
		uint8_t side_turns = positive_mod(turn_sides_table[move.face][target_side] -
										turn_sides_table[move.face][side], NUM_SIDES);

		// clear the bits of the side pieces
		cube->state[target_sface] &= ~turn_mask_table[move.face][target_side];
		// set the bits of the side pieces 
		cube->state[target_sface] |=  turn_mask_table[move.face][target_side] & 
										rolq(og_sfaces[side], side_turns*16);
	}
}

void print_face(uint64_t face_bits) {
	printf("%c%c%c\n%c %c\n%c%c%c\n", 
			get_piece(face_bits, 0), get_piece(face_bits, 1), get_piece(face_bits, 2),
			get_piece(face_bits, 7),                          get_piece(face_bits, 3),
			get_piece(face_bits, 6), get_piece(face_bits, 5), get_piece(face_bits, 4)); 
}

void print_cube(cube_s cube) {
	printf("    %c%c%c\n    %c%c%c\n    %c%c%c\n", 
			get_piece(cube.state[FACE_U], 0), get_piece(cube.state[FACE_U], 1), get_piece(cube.state[FACE_U], 2), 
			get_piece(cube.state[FACE_U], 7), CHAR_U,                           get_piece(cube.state[FACE_U], 3), 
			get_piece(cube.state[FACE_U], 6), get_piece(cube.state[FACE_U], 5), get_piece(cube.state[FACE_U], 4));
	printf("%c%c%c %c%c%c %c%c%c %c%c%c\n",
			get_piece(cube.state[FACE_L], 0), get_piece(cube.state[FACE_L], 1), get_piece(cube.state[FACE_L], 2),
			get_piece(cube.state[FACE_F], 0), get_piece(cube.state[FACE_F], 1), get_piece(cube.state[FACE_F], 2),
			get_piece(cube.state[FACE_R], 0), get_piece(cube.state[FACE_R], 1), get_piece(cube.state[FACE_R], 2),
			get_piece(cube.state[FACE_B], 0), get_piece(cube.state[FACE_B], 1), get_piece(cube.state[FACE_B], 2));
	printf("%c%c%c %c%c%c %c%c%c %c%c%c\n",
			get_piece(cube.state[FACE_L], 7), CHAR_L,  get_piece(cube.state[FACE_L], 3),
			get_piece(cube.state[FACE_F], 7), CHAR_F,  get_piece(cube.state[FACE_F], 3),
			get_piece(cube.state[FACE_R], 7), CHAR_R,  get_piece(cube.state[FACE_R], 3),
			get_piece(cube.state[FACE_B], 7), CHAR_B,  get_piece(cube.state[FACE_B], 3));
	printf("%c%c%c %c%c%c %c%c%c %c%c%c\n",
			get_piece(cube.state[FACE_L], 6), get_piece(cube.state[FACE_L], 5), get_piece(cube.state[FACE_L], 4),
			get_piece(cube.state[FACE_F], 6), get_piece(cube.state[FACE_F], 5), get_piece(cube.state[FACE_F], 4),
			get_piece(cube.state[FACE_R], 6), get_piece(cube.state[FACE_R], 5), get_piece(cube.state[FACE_R], 4),
			get_piece(cube.state[FACE_B], 6), get_piece(cube.state[FACE_B], 5), get_piece(cube.state[FACE_B], 4));
	printf("    %c%c%c\n    %c%c%c\n    %c%c%c\n\n", 
			get_piece(cube.state[FACE_D], 0), get_piece(cube.state[FACE_D], 1), get_piece(cube.state[FACE_D], 2), 
			get_piece(cube.state[FACE_D], 7), CHAR_D,                           get_piece(cube.state[FACE_D], 3), 
			get_piece(cube.state[FACE_D], 6), get_piece(cube.state[FACE_D], 5), get_piece(cube.state[FACE_D], 4));
}
