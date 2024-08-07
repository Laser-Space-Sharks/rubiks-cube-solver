#include "move.h"

move_list_s init_move_list() {
	move_list_s move_list;
	move_list.moves = (move_s*)malloc(INIT_MOVE_LIST_SIZE * sizeof(move_s));
	move_list.length = 0;
	move_list.size = INIT_MOVE_LIST_SIZE;

	return move_list;
}

// return a new move_list_s with a distinct array of moves copied from move_list
move_list_s copy_move_list(move_list_s move_list) {
	move_list_s copy_list;

	// copy the data from the other move_list onto a new, distinct array with its own pointer
	copy_list.moves = (move_s*)malloc(move_list.size * sizeof(move_s));
	(void)memcpy(copy_list.moves, move_list.moves, move_list.length * sizeof(move_s));

	copy_list.size = move_list.size;
	copy_list.length = move_list.length;

	return copy_list;
}

void insert_move(move_list_s *move_list, move_s move, size_t index) {
	// reallocate the move list if needed
	if (move_list->length == move_list->size) {
		move_list->size *= 2;
		move_list->moves = (move_s*)realloc(move_list->moves, 
									  		move_list->size * sizeof(move_s));
	}

	// if this is an insertion, move everything after index one space up
	if (index != move_list->length) {
		(void)memmove(move_list->moves + index + 1, move_list->moves + index, 
						(move_list->length - (index + 1)) * sizeof(move_s));
	}

	move_list->moves[index] = move;
	move_list->length++;
}

void delete_move(move_list_s *move_list, size_t index) {
	// if this isn't just a "pop", bring moves ahead of index one index down
	// and if it is, there's no need to do anything besides decrement length
	if (index != move_list->length - 1) {
		(void)memmove(move_list->moves + index, move_list->moves + index + 1,
					  sizeof(move_s) * (move_list->length - (index + 1)));
	}
	move_list->length--;

	// for memory leak protection: decrease the size of the move list if length <= 1/4 size
	// and the length is still greater than INIT_MOVE_LIST_SIZE
	if (move_list->length >= INIT_MOVE_LIST_SIZE && move_list->length <= move_list->size / 4) {
		move_list->size *= 1/2;
		move_list->moves = (move_s*)realloc(move_list->moves, sizeof(move_s) * move_list->size);
	}
}

void free_move_list(move_list_s *move_list) {
	free(move_list->moves);
	move_list->moves = NULL;
	move_list->size = move_list->length = 0;
}
