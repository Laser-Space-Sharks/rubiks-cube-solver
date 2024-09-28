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

char get_piece(uint64_t face, uint8_t index) {
    return get_char((face >> (8 * index) & 0xFF));
}

void print_piece(uint64_t face, uint8_t index) {
    char piece_char = get_piece(face, index);
    switch (piece_char) {
        case CHAR_U:
            printf("\033[31;93m%c", piece_char); // ansi code for 'bright yellow'
            break;
        case CHAR_R:
            printf("\033[31;91m%c", piece_char); // ansi code for 'bright red'
            break;
        case CHAR_F:
            printf("\033[31;94m%c", piece_char); // ANSI code for 'bright blue'
            break;
        case CHAR_L:
            printf("\033[31;33m%c", piece_char); // ANSI code for 'yellow'
            break;
        case CHAR_B:
            printf("\033[31;32m%c", piece_char); // ANSI code for 'green'
            break;
        case CHAR_D:
            printf("\033[31;97m%c", piece_char); // ANSI code for 'bright white'
            break;
        default:
            printf("\033[31;90m%c", piece_char); // ANSI code for 'bright black'
            break;
    }
}
