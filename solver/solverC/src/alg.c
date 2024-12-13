#include "alg.h"
#include "lookup_tables.h"

alg_s *alg_create(size_t size) {
    alg_s *alg = (alg_s*)malloc(sizeof(alg_s));

    alg->moves = (move_s*)malloc(size * sizeof(move_s));
    alg->length = 0;
    alg->size = size;

    return alg;
}

// return a pointer to a new alg_s with a distinct yet identical array of the alg from src
alg_s* alg_copy(const alg_s *src) {
    if (!src) return NULL;

    alg_s *copy = alg_create(src->length);

    (void)memcpy(copy->moves, src->moves, src->length * sizeof(move_s));
    copy->length = src->length;

    return copy;
}

alg_s alg_static_copy(const alg_s *src) {
    alg_s copy;
    copy.moves = malloc(sizeof(move_s) * src->length);
    (void)memcpy(copy.moves, src->moves, src->length * sizeof(move_s));
    copy.length = src->length;
    copy.size   = src->length;

    return copy;
}

void alg_free(alg_s *alg) {
    if (alg == NULL) {
        return;
    }

    free(alg->moves);
    free(alg);
}

bool alg_insert(alg_s *alg, move_s move, size_t index) {
    // don't try to insert if index is out of bounds
    if (index > alg->length) {
        return false;
    }

	// reallocate the move moves if needed
    if (alg->length == alg->size) {
        alg->size *= 2;
        alg->moves = (move_s*)realloc(alg->moves,
                                       alg->size * sizeof(move_s));
    }

    // if this isn't an append, move everything after index one space up
    if (index != alg->length) {
        (void)memmove(alg->moves + index + 1, alg->moves + index,
                      (alg->length - index) * sizeof(move_s));
    }

    alg->moves[index] = move;
    alg->length++;

    // insertion was successful
    return true;
}

bool alg_delete(alg_s *alg, size_t index) {
    // don't try to delete if index is out of bounds
    if (index >= alg->length) {
        return false;
    }

    // if this isn't just a "pop", bring alg ahead of index one index down
    // and if it is, there's no need to do anything besides decrement length
    if (index != alg->length - 1) {
        (void)memmove(alg->moves + index, alg->moves + index + 1,
                      sizeof(move_s) * (alg->length - (index + 1)));
    }
    alg->length--;

    // for memory leak protection: decrease the size of the move moves if length <= 1/4 size
    // and the length is still greater than INIT_alg_SIZE
    if (alg->length >= MIN_LIST_RESIZE && alg->length <= alg->size/4) {
        alg->size /= 2;
        alg->moves = (move_s*)realloc(alg->moves, sizeof(move_s) * alg->size);
    }

    // deletion was successful
    return true;
}

// reverse a alg_s "alg" in place
void alg_invert(alg_s *alg) {
    if (alg == NULL) { 
        return;
    }

    if (alg->moves == NULL || alg->length == 0 || alg->size == 0) {
        return;
    }

    if (alg->length == 1) {
        alg->moves[0].turns = mod4(-alg->moves[0].turns);
        return;
    }

    for (size_t i = 0, j = alg->length-1; i <= j; i++, j--) {
        move_s tmp  = alg->moves[i];
        move_s tmp2 = alg->moves[j];

        tmp.turns  = mod4(-tmp.turns);
        tmp2.turns = mod4(-tmp2.turns);

        alg->moves[i] = tmp2;
        alg->moves[j] = tmp;
    }
}

size_t alg_lookup(const alg_s *alg, move_s move) {
    for (size_t i = 0; i < alg->length; i++) {
        if (alg->moves[i].face == move.face &&
            mod4(alg->moves[i].turns) == mod4(move.turns)) {
            return i;
        }
    }
    return -1;
}

// simplify move sequences in the move moves
void alg_simplify(alg_s *alg) {
    if (alg == NULL) {
        return;
    }

    size_t idx = 0;
    size_t idx2 = 1;
    while (idx2 < alg->length) {
        while (alg->moves[idx2].face == opposite_faces[alg->moves[idx].face]
            && idx2 < alg->length - 1 && idx2 > 0) {
            idx2 += 1;
        }

        while (alg->moves[idx].face == alg->moves[idx2].face) {
            alg->moves[idx].turns += alg->moves[idx2].turns;
            alg_delete(alg, idx2);

            // if we can't delete any more alg after this, stop
            if (idx2 >= alg->length || idx >= alg->length) {
                break;
            }
        }

        /* if our combined move consititutes zero alg, get rid of it
         * and if we can, check the move before this deleted move for
         * simplification. Additionally, if there's a sequence of
         * consequtive same/opposite face alg, keep moving idx back
         * to account for new potential simplifications of earlier alg.
         */
        if (mod4(alg->moves[idx].turns) == 0) {
            alg_delete(alg, idx);
            while (--idx > 0) {
                if (!(alg->moves[idx].face == opposite_faces[alg->moves[idx - 1].face] ||
                    alg->moves[idx].face == alg->moves[idx-1].face)) {
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

alg_s* alg_from_alg_str(const char *alg_str) {
    if (alg_str == NULL) {
        return alg_create(0);
    }

    alg_s *alg = alg_create(2 * MIN_LIST_RESIZE);

    move_s move = (move_s) {
        .face = FACE_NULL,
        .turns = 1
    };

    for (size_t idx = 0; alg_str[idx] != '\0'; idx++) {
        if (alg_str[idx] == ' ' || alg_str[idx] == '\n' || alg_str[idx] == '\t') {
            if (move.face != FACE_NULL && mod4(move.turns) != 0) {
                alg_insert(alg, move, alg->length);

                // reset the move to add
                move.face = FACE_NULL;
                move.turns = 1;
            }
            continue;
        }

        switch (alg_str[idx]) {
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
                if (alg_str[idx] >= '0' && alg_str[idx] <= '9') {
                    move.turns = mod4(alg_str[idx] - '0');
                    break;
                }
                // if we're here we've hit an invalid character, alg
                // will store the generated move moves up until that character
                return 0;
        }
    }

    // if we've processed a clockwise quarter turn that comes at the end of
    // the move string we need to add it to the alg
    if (move.face != FACE_NULL) {
        alg_insert(alg, move, alg->length);
    }

    return alg;
}

void alg_rotate_on_y(alg_s *alg, uint8_t y_turns) {
    if (alg == NULL || alg->moves == NULL) {
        return;
    }

    for (int i = 0; i < alg->length; i++) {
        alg->moves[i].face = rotate_on_y[mod4(y_turns)][alg->moves[i].face];
    }
}

move_s* alg_concat(alg_s *dest, const alg_s *src) {
    if (!dest || !src) {
        return NULL;
    }

    size_t new_len = dest->length + src->length;
    if (new_len > dest->size) {
        move_s *tmp = (move_s*)realloc(dest->moves, sizeof(move_s)*new_len);
        if (!tmp) {
            return NULL;
        }
        dest->moves = tmp;
        dest->size = new_len;
    }

    for (size_t idx = 0; idx < src->length; idx++) {
        dest->moves[dest->length+idx] = src->moves[idx];
    }

    dest->length = new_len;
    return dest->moves;
}
