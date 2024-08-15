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
