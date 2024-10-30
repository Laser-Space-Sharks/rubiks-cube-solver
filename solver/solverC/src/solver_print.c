#include "solver_print.h"

/*
 * ASCII print functions for pieces, faces, and cubes
 */

char get_char(face_e face) {
    switch (face) {
        case FACE_U:
            return CHAR_U;
        case FACE_R:
            return CHAR_R;
        case FACE_F:
            return CHAR_F;
        case FACE_L:
            return CHAR_L;
        case FACE_B:
            return CHAR_B;
        case FACE_D:
            return CHAR_D;
        default:
            return 'X';
	}
}

char get_piece(uint32_t face, uint8_t index) {
    return get_char((face >> (4 * index) & 0xF));
}

void print_piece(uint32_t face, uint8_t index) {
    char piece_char = get_piece(face, index);
    switch (piece_char) {
        case CHAR_U:
            printf("\033[31;93m%c\033[0m", piece_char); // ANSI 4-bit color code for 'bright yellow'
            break;
        case CHAR_R:
            printf("\033[31;91m%c\033[0m", piece_char); // ANSI 4-bit color code for 'bright red'
            break;
        case CHAR_F:
            printf("\033[31;94m%c\033[0m", piece_char); // ANSI 4-bit color code for 'bright blue'
            break;
        case CHAR_L:
            printf("\033[31;33m%c\033[0m", piece_char); // ANSI 4-bit color code for 'yellow'
            break;
        case CHAR_B:
            printf("\033[31;32m%c\033[0m", piece_char); // ANSI 4-bit color code for 'green'
            break;
        case CHAR_D:
            printf("\033[31;97m%c\033[0m", piece_char); // ANSI 4-bit color code for 'bright white'
            break;
        default:
            printf("\033[31;90m%c\033[0m", piece_char); // ANSI 4-bit color code for 'bright black'
            break;
    }
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

void print_move_list(const move_list_s *moves) {
	for (size_t idx = 0; idx < moves->length; idx++) {
		char move_char = get_char(moves->list[idx].face);
		char turn_char = '\0';

		switch (positive_mod(moves->list[idx].turns, 4)) {
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

		if (moves->list[idx].face >= NUM_FACES) {
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
