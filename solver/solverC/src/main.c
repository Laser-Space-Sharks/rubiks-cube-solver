#include <bits/time.h>
#include <time.h>
#include <unistd.h>

#include "solver.h"
#include "cube.h"
#include "move.h"

const cube_s SOLVED_BITCUBE = {
	.state = {
		(uint64_t)FACE_U << 56 | (uint64_t)FACE_U << 48 | (uint64_t)FACE_U << 40 | 
		(uint64_t)FACE_U << 32 |                          (uint64_t)FACE_U << 24 | 
		(uint64_t)FACE_U << 16 | (uint64_t)FACE_U <<  8 | (uint64_t)FACE_U,

		(uint64_t)FACE_R << 56 | (uint64_t)FACE_R << 48 | (uint64_t)FACE_R << 40 | 
		(uint64_t)FACE_R << 32 |                          (uint64_t)FACE_R << 24 | 
		(uint64_t)FACE_R << 16 | (uint64_t)FACE_R <<  8 | (uint64_t)FACE_R,
		
		(uint64_t)FACE_F << 56 | (uint64_t)FACE_F << 48 | (uint64_t)FACE_F << 40 | 
		(uint64_t)FACE_F << 32 |                          (uint64_t)FACE_F << 24 | 
		(uint64_t)FACE_F << 16 | (uint64_t)FACE_F <<  8 | (uint64_t)FACE_F,
		
		(uint64_t)FACE_L << 56 | (uint64_t)FACE_L << 48 | (uint64_t)FACE_L << 40 | 
		(uint64_t)FACE_L << 32 |                          (uint64_t)FACE_L << 24 | 
		(uint64_t)FACE_L << 16 | (uint64_t)FACE_L <<  8 | (uint64_t)FACE_L,
		
		(uint64_t)FACE_B << 56 | (uint64_t)FACE_B << 48 | (uint64_t)FACE_B << 40 | 
		(uint64_t)FACE_B << 32 |                          (uint64_t)FACE_B << 24 | 
		(uint64_t)FACE_B << 16 | (uint64_t)FACE_B <<  8 | (uint64_t)FACE_B,
		
		(uint64_t)FACE_D << 56 | (uint64_t)FACE_D << 48 | (uint64_t)FACE_D << 40 | 
		(uint64_t)FACE_D << 32 |                          (uint64_t)FACE_D << 24 | 
		(uint64_t)FACE_D << 16 | (uint64_t)FACE_D <<  8 | (uint64_t)FACE_D
	}
};

int main(int argc, char *argv[]) {
	cube_s cube = SOLVED_BITCUBE;
	move_s u = {
		.face  = FACE_U,
		.turns = 1
	};

	move_s r = {
		.face = FACE_R,
		.turns = 1
	};

	move_s f = {
		.face = FACE_F,
		.turns = 1
	};

	move_s l = {
		.face = FACE_L,
		.turns = 1
	};

	move_s b = {
		.face = FACE_B,
		.turns = 1
	};

	move_s d = {
		.face = FACE_D,
		.turns = 1
	};

	move_s fprime = {
		.face = FACE_F,
		.turns = -1
	};

	move_s rprime = {
		.face = FACE_R,
		.turns = -1
	};

	move_s uprime = {
		.face = FACE_U,
		.turns = -1
	};

	move_s dprime = {
		.face = FACE_D,
		.turns = -1
	};

	move_s lprime = {
		.face = FACE_L,
		.turns = -1
	};

	move_list_s moves;
	init_move_list(&moves, 10);
	insert_move(&moves, f, moves.length);
	insert_move(&moves, u, moves.length);
	insert_move(&moves, d, moves.length);
	insert_move(&moves, d, moves.length);
	insert_move(&moves, d, moves.length);
	insert_move(&moves, d, moves.length);
	insert_move(&moves, l, moves.length);
	insert_move(&moves, lprime, moves.length);
	insert_move(&moves, u, moves.length);
	insert_move(&moves, u, moves.length);
	insert_move(&moves, b, moves.length);
	insert_move(&moves, b, moves.length);
	insert_move(&moves, f, moves.length);

	print_move_list(moves);

	struct timespec start, stop;
	uint64_t accum;

	clock_gettime(CLOCK_REALTIME, &start);
	simplify_move_list(&moves);
	clock_gettime(CLOCK_REALTIME, &stop);
	
	accum = stop.tv_nsec - start.tv_nsec;
	printf("Simplified move list in %ldns\n", accum);

	print_move_list(moves);

	free_move_list(&moves);
	return 0;
}
