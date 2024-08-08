#include "move.h"

void init_move_list(move_list_s* move_list, size_t size) {
	move_list->list = (move_s*)malloc(size * sizeof(move_s));
	move_list->length = 0;
	move_list->size = size;
}

// return a new move_list_s with a distinct yet identical array of the moves from move_list
void copy_move_list(move_list_s *cpy, move_list_s *src) {
	if (cpy->list != NULL) {
		free_move_list(cpy);
		init_move_list(cpy, src->size);
	}
	
	(void)memcpy(cpy->list, src->list, src->length * sizeof(move_s));

	cpy->size = src->size;
	cpy->length = src->length;
}

void insert_move(move_list_s *move_list, move_s move, size_t index) {
	// reallocate the move list if needed
	if (move_list->length == move_list->size) {
		move_list->size *= 2;
		move_list->list = (move_s*)realloc(move_list->list, 
									  		move_list->size * sizeof(move_s));
	}

	// if this is an insertion, move everything after index one space up
	if (index != move_list->length) {
		(void)memmove(move_list->list + index + 1, move_list->list + index, 
						(move_list->length - (index + 1)) * sizeof(move_s));
	}

	move_list->list[index] = move;
	move_list->length++;
}

void delete_move(move_list_s *move_list, size_t index) {
	// if this isn't just a "pop", bring moves ahead of index one index down
	// and if it is, there's no need to do anything besides decrement length
	if (index != move_list->length - 1) {
		(void)memmove(move_list->list + index, move_list->list + index + 1,
					  sizeof(move_s) * (move_list->length - (index + 1)));
	}
	move_list->length--;

	// for memory leak protection: decrease the size of the move list if length <= 1/4 size
	// and the length is still greater than INIT_MOVE_LIST_SIZE
	if (move_list->length >= MIN_LIST_RESIZE && move_list->length <= move_list->size/4) {
		move_list->size *= 1/2;
		move_list->list = (move_s*)realloc(move_list->list, sizeof(move_s) * move_list->size);
	}
}

void free_move_list(move_list_s *move_list) {
	free(move_list->list);
	move_list->list = NULL;
	move_list->size = move_list->length = 0;
}
