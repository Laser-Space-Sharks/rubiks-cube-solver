#include "move.h"

void init_move_list(move_list_s* move_list, size_t size) {
	move_list->list = (move_s*)malloc(size * sizeof(move_s));
	move_list->length = 0;
	move_list->size = size;
}

// return a new move_list_s with a distinct yet identical array of the moves from move_list
void copy_move_list(move_list_s *cpy, move_list_s *src) {
	free_move_list(cpy);
	init_move_list(cpy, src->size);
	
	(void)memcpy(cpy->list, src->list, src->length * sizeof(move_s));

	cpy->size = src->size;
	cpy->length = src->length;
}

int insert_move(move_list_s *move_list, move_s move, size_t index) {
	// don't try to insert if index is out of bounds
	if (index > move_list->length) {
		return 0;
	}

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

	// insertion was successful
	return 1;
}

int delete_move(move_list_s *move_list, size_t index) {
	// don't try to delete if index is out of bounds
	if (index >= move_list->length) {
		return 0;
	}

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
		move_list->size /= 2;
		move_list->list = (move_s*)realloc(move_list->list, sizeof(move_s) * move_list->size);
	}

	// deletion was successful
	return 1;
}

void free_move_list(move_list_s *move_list) {
	free(move_list->list);
	move_list->list = NULL;
	move_list->size = move_list->length = 0;
}

// opposite face lookup for the simplifier
const face_e opposite_faces[NUM_FACES] = {
	FACE_D, FACE_L, FACE_B, FACE_R, FACE_F, FACE_U
};

// simplify sequences in the move list
void simplify_move_list(move_list_s* moves) {
	size_t idx = 0;
	size_t idx2 = 1;
	while (idx2 < moves->length) {
		while (moves->list[idx2].face == opposite_faces[moves->list[idx].face]
				&& idx2 < moves->length - 1 && idx2 > 0) {
			idx2 += 1;
		}

		while (moves->list[idx].face == moves->list[idx2].face) {
			moves->list[idx].turns += moves->list[idx2].turns;
			delete_move(moves, idx2);

			// if we can't delete any more moves after this, stop
			if (idx2 >= moves->length || idx >= moves->length) {
				break;
			}
		}

		// if our combined move consititutes zero moves, get rid of it
		// and if we can, check the move before this deleted move for
		// simplification. Additionally, if there's a sequence of 
		// consequtive same/opposite face moves, keep moving idx back
		// to account for new simplifications of earlier moves.
		if (moves->list[idx].turns % 4 == 0) {
			delete_move(moves, idx);
			while (--idx > 0) {
				if (!(moves->list[idx].face == opposite_faces[moves->list[idx - 1].face] ||
					  moves->list[idx].face == moves->list[idx-1].face)) {
					break;
				}
			}
			printf("new idx: %ld\n", idx);
			idx2 = idx + 1;
			continue;
		}

		idx++;
		idx2 = idx + 1;
	}
}

void print_move(move_s move) {
	char move_char = get_char(move.face);
	char turn_char;

	switch (positive_mod(move.turns, 4)) {
		case 0:
			turn_char = '0';
			break;
		case 1:
			turn_char = ' ';
			break;
		case 2:
			turn_char = '2';
			break;
		case 3:
			turn_char = '\'';
			break;
	}

	if (move.face >= NUM_FACES) {
		printf("INVALID MOVE\n");
	}

	printf("%c%c\n", move_char, turn_char);
}

void print_move_list(move_list_s move_list) {
	for (size_t idx = 0; idx < move_list.length; idx++) {
		char move_char = get_char(move_list.list[idx].face);
		char turn_char;

		switch (positive_mod(move_list.list[idx].turns, 4)) {
			case 0:
				turn_char = '0';
				break;
			case 1:
				turn_char = ' ';
				break;
			case 2:
				turn_char = '2';
				break;
			case 3:
				turn_char = '\'';
				break;
		}

		if (move_list.list[idx].face >= NUM_FACES) {
			printf("Invalid move in move list\n");
			return;
		}

		printf("%c%c", move_char, turn_char);
	}
	printf("\n");
}
