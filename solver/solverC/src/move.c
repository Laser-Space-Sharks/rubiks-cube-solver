#include "move.h"
#include "main.h"

int init_move_list(move_list_s *moves, size_t size) {
    moves->list = calloc(size, sizeof(move_s));
    moves->length = 0;
    moves->size = size;
    return 1;
}

// return a new move_list_s with a distinct yet identical array of the moves from src
int copy_move_list(move_list_s *cpy, move_list_s *src) {
    free_move_list(cpy);
    init_move_list(cpy, src->size);

    (void)memcpy(cpy->list, src->list, src->length * sizeof(move_s));

    cpy->size = src->size;
    cpy->length = src->length;
    return 1;
}

void free_move_list(move_list_s *moves) {
    free(moves->list);
    moves->list = NULL;
    moves->size = moves->length = 0;
}

int insert_move(move_list_s *moves, move_s move, size_t index) {
    // don't try to insert if index is out of bounds
    if (index > moves->length) {
        return 0;
    }

	// reallocate the move list if needed
    if (moves->length == moves->size) {
        moves->size *= 2;
        moves->list = (move_s*)realloc(moves->list,
                                       moves->size * sizeof(move_s));
    }

    // if this isn't an append, move everything after index one space up
    if (index != moves->length) {
        (void)memmove(moves->list + index + 1, moves->list + index,
                      (moves->length - index) * sizeof(move_s));
    }

    moves->list[index] = move;
    moves->length++;

    // insertion was successful
    return 1;
}

int delete_move(move_list_s *moves, size_t index) {
    // don't try to delete if index is out of bounds
    if (index >= moves->length) {
        return 0;
    }

    // if this isn't just a "pop", bring moves ahead of index one index down
    // and if it is, there's no need to do anything besides decrement length
    if (index != moves->length - 1) {
        (void)memmove(moves->list + index, moves->list + index + 1,
                      sizeof(move_s) * (moves->length - (index + 1)));
    }
    moves->length--;

	// for memory leak protection: decrease the size of the move list if length <= 1/4 size
	// and the length is still greater than INIT_MOVE_LIST_SIZE
    if (moves->length >= MIN_LIST_RESIZE && moves->length <= moves->size/4) {
        moves->size /= 2;
        moves->list = (move_s*)realloc(moves->list, sizeof(move_s) * moves->size);
    }

    // deletion was successful
    return 1;
}

// reverse a move_list_s "moves" in place
void invert_move_list(move_list_s *moves) {
    for (size_t i = 0, j = moves->length-1; i <= j; i++, j--) {
        move_s tmp  = moves->list[i];
        move_s tmp2 = moves->list[j];

        tmp.turns  = positive_mod(-tmp.turns,  4);
        tmp2.turns = positive_mod(-tmp2.turns, 4);

        moves->list[i] = tmp2;
        moves->list[j] = tmp;
    }
}

int in_move_list(move_list_s *moves, move_s move) {
    for (size_t i = 0; i < moves->length; i++) {
        if (moves->list[i].face == move.face &&
            positive_mod(moves->list[i].turns, 4) == positive_mod(move.turns, 4)) {
            return i;
        }
    }
    return -1;
}

int face_in_move_list(move_list_s *moves, face_e face) {
    for (size_t i = 0; i < moves->length; i++) {
        if (moves->list[i].face == face) {
            return i;
        }
    }
    return -1;
}

// simplify move sequences in the move list
void simplify_move_list(move_list_s *moves) {
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

        /* if our combined move consititutes zero moves, get rid of it
        * and if we can, check the move before this deleted move for
        * simplification. Additionally, if there's a sequence of
        * consequtive same/opposite face moves, keep moving idx back
        * to account for new potential simplifications of earlier moves.
        */
        if (moves->list[idx].turns % 4 == 0) {
            delete_move(moves, idx);
            while (--idx > 0) {
			    if (!(moves->list[idx].face == opposite_faces[moves->list[idx - 1].face] ||
                    moves->list[idx].face == moves->list[idx-1].face)) {
                    break;
                }
            }
            idx2 = idx + 1;
            continue;
        }

        idx++;
        idx2 = idx + 1;
    }
}

int move_list_from_move_str(move_list_s *moves, char *move_str) {
    free_move_list(moves);

    init_move_list(moves, 10);

    move_s move = (move_s) {
        .face = FACE_NULL,
        .turns = 1
    };

    size_t idx;
    for (idx = 0; move_str[idx] != '\0'; idx++) {
        if (move_str[idx] == ' ') {
            if (move.face != FACE_NULL && move.turns % 4 != 0) {
                insert_move(moves, move, moves->length);

                // reset the move to add
                move.face = FACE_NULL;
                move.turns = 1;
            }
            continue;
        }

        switch (move_str[idx]) {
            case 'U':
                move.face = FACE_U;
                break;
            case 'R':
                move.face = FACE_R;
                break;
            case 'F':
                move.face = FACE_F;
                break;
            case 'L':
                move.face = FACE_L;
                break;
            case 'B':
                move.face = FACE_B;
                break;
            case 'D':
                move.face = FACE_D;
                break;
            case '\'': // ' (prime) move
                move.turns = -1;
                break;
            default:
                if (move_str[idx] >= '0' && move_str[idx] <= '9') {
                    move.turns = (move_str[idx] - '0') % 4;
                    break;
                }
                // if we're here we've hit an invalid character, moves
                // will store the generated move list up until that character
                return 0;
        }
    }

    // if we've processed a clockwise quarter turn that comes at the end of
    // the move string we need to add it to the moves
    if (move.face != FACE_NULL) {
        insert_move(moves, move, moves->length);
    }
    return 1;
}

void print_move(move_s move) {
	char move_char = get_char(move.face);
	char turn_char;

	switch (positive_mod(move.turns, 4)) {
		case 0:
			turn_char = '0';
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

	printf("%c", move_char);
	if (move.turns != 1) {
	   printf("%c\n", turn_char);
	}
}

void print_move_list(move_list_s moves) {
	for (size_t idx = 0; idx < moves.length; idx++) {
		char move_char = get_char(moves.list[idx].face);
		char turn_char = '\0';

		switch (positive_mod(moves.list[idx].turns, 4)) {
			case 0:
				turn_char = '0';
				break;
			case 2:
				turn_char = '2';
				break;
			case 3:
				turn_char = '\'';
				break;
		}

		if (moves.list[idx].face >= NUM_FACES) {
			printf("Invalid move in move list\n");
			return;
		}

		printf("%c", move_char);
		if (turn_char) {
    		printf("%c", turn_char);
		}
		printf(" ");
	}
	printf("\n");
}
