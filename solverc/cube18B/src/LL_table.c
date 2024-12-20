#include "cube18B.h"
#include "solver_print.h"
#include "LL_table.h"
#include "shift_cube.h"

typedef struct {
    cube18B_1LLL_s key;
    alg_s alg;
} LL_entry_s;

typedef struct LL_table {
    size_t entries;
    size_t size;

    LL_entry_s *table;
} LL_table_s;

LL_table_s* LL_table_create(size_t size) {
    LL_table_s *ct = (LL_table_s*)malloc(sizeof(LL_table_s));

    ct->table = (LL_entry_s*)calloc(size, sizeof(LL_entry_s));

    ct->entries = 0;
    ct->size    = size;
    return ct;
}

size_t LL_table_hash(const LL_table_s* ct, const cube18B_1LLL_s* key) {
    size_t hash = 0;
    for (uint8_t ind = 0; ind < 6; ind++) {
        hash ^= key->cubies[ind];
        hash  = rolq(hash, 3);
    }

    return hash % ct->size;
}

bool LL_table_insert(LL_table_s *ct, const cube18B_1LLL_s* key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    size_t hash = LL_table_hash(ct, key);
    size_t index = hash;

    // linear probing
    while (ct->table[index].alg.moves != NULL) {
        if (compare_cube18B_1LLL(&(ct->table[index].key), key)) {
            return false;
        }

        index++;
        if (index == hash) {
            return false;
        }

        if (index >= ct->size) {
            index = 0;
        }
    }


    ct->table[index].key  = *key;
    ct->table[index].alg = alg_static_copy(moves);

    ct->entries++;

    return true;
}

static inline size_t LL_table_get_cube_index(const LL_table_s *ct, const cube18B_1LLL_s *cube) {
    if (ct == NULL || cube == NULL) {
        return ct->size;
    }

    size_t hash = LL_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].alg.moves != NULL) {
        if (compare_cube18B_1LLL(&(ct->table[index].key), cube)) {
            return index;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ct->size) ? 0 : index + 1;
    }

    return ct->size;
}

const alg_s* LL_table_lookup(const LL_table_s *ct, const cube18B_1LLL_s *cube) {
    size_t index = LL_table_get_cube_index(ct, cube);

    return (index == ct->size) ? NULL : &ct->table[index].alg;
}

void LL_table_free(LL_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        free(ct);
        return;
    }

    for (size_t index = 0; index < ct->size; index++) {
        if (ct->table[index].alg.moves != NULL) {
            free(ct->table[index].alg.moves);
        }
    }

    ct->entries = 0;

    free(ct->table);
    free(ct);
}

void LL_table_print(LL_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) {
            printf("%10zu ", idx);
            print_cube18B_1LLL(&(ct->table[idx].key));
            print_alg(&(ct->table[idx].alg));
        } else if (ct->table[idx-1].alg.moves != NULL) {
            printf("                                    ...                                    \n");
        }
    }
}

size_t LL_table_entries(const LL_table_s *ct) {
    return ct->entries;
}

size_t LL_table_size(const LL_table_s *ct) {
    return ct->size;
}