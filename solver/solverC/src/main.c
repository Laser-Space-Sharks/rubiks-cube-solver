#include <time.h>
#include <unistd.h>

#include "solver.h"
#include "solver.h"
#include "cube.h"

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

struct timespec timer_start();
long timer_end(struct timespec start_time);

int main(int argc, char *argv[]) {
	cube_s cube = SOLVED_BITCUBE;
	move_s move_u = {
		.face  = FACE_U,
		.turns = 1
	};

	move_s move_r = {
		.face = FACE_R,
		.turns = 1
	};

	move_s move_f = {
		.face = FACE_F,
		.turns = 1
	};

	move_s move_l = {
		.face = FACE_L,
		.turns = 1
	};

	move_s move_b = {
		.face = FACE_B,
		.turns = 1
	};

	move_s move_d = {
		.face = FACE_D,
		.turns = 1
	};

	move_s move_fprime = {
		.face = FACE_F,
		.turns = -1
	};

	move_s move_rprime = {
		.face = FACE_R,
		.turns = -1
	};

	move_s move_uprime = {
		.face = FACE_U,
		.turns = -1
	};

	uint32_t max_iters = 100000;
	struct timespec timer = timer_start();
	for (size_t iter = 0; iter < max_iters; iter++) {
		apply_move(&cube, move_f);
	}
	long diffInNanos = timer_end(timer);
	printf("Time to execute %d moves: %ldns\n", max_iters, diffInNanos);
	printf("Average of %ldns per move\n", diffInNanos/max_iters);
	return 0;
}

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

