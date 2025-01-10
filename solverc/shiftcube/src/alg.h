#ifndef ALG_H
#define ALG_H

#include "main.h"
#include "move.h"


typedef struct {
    uint8_t size;
    uint8_t length;
    move_t *moves;
} alg_s;

typedef struct {
    size_t size;
    size_t num_algs;
    alg_s *list;
} alg_list_s;

// no particular reason for this choice, 4 is just a nice and small power of two
#define MIN_LIST_RESIZE 4

alg_s* alg_create(size_t size);
alg_s* alg_copy(const alg_s *src);
alg_s  alg_static_copy(const alg_s *src);
void   alg_free(alg_s *alg);

bool alg_insert(alg_s *moves, move_e move, size_t index);
bool alg_append(alg_s *alg, move_e move);
bool alg_delete(alg_s *moves, size_t index);
bool alg_pop(alg_s *moves);
size_t alg_lookup(const alg_s *alg, move_e move);
alg_s* alg_from_alg_str(const char *alg_str);
alg_s* alg_from_str(const char *str);

void alg_invert(alg_s *alg);
void alg_simplify(alg_s *alg);
void alg_rotate_on_y(alg_s *alg, uint8_t y_turns);

move_e* alg_concat(alg_s *dest, const alg_s *src);

bool simplified_alg_compare_forms(const alg_s* a, const alg_s* b);
bool alg_compare(const alg_s* a, const alg_s* b);
alg_list_s* get_alg_family(const alg_s* alg);

alg_list_s* alg_list_create(size_t num_algs);
void alg_list_append(alg_list_s *alg_list, const alg_s *alg);
void alg_list_free(alg_list_s *alg_list);

alg_list_s* alg_list_from_file(const char *filepath);

static const alg_s NULL_ALG = {
    (size_t)0,
    (size_t)0,
    (move_t*)NULL
};

#endif // ALG_H
