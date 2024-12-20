#include "cube18B.h"
#include "solver_print.h"
#include "xcross_table.h"
#include "shift_cube.h"

typedef struct {
    cube18B_xcross_s key;
    alg_list_s algs;
} xcross_entry_s;

typedef struct xcross_table {
    size_t entries;
    size_t size;

    xcross_entry_s *table;
} xcross_table_s;

xcross_table_s* xcross_table_create(size_t size) {
    xcross_table_s *ct = (xcross_table_s*)malloc(sizeof(xcross_table_s));

    ct->table = (xcross_entry_s*)calloc(size, sizeof(xcross_entry_s));

    ct->entries = 0;
    ct->size    = size;
    return ct;
}

size_t xcross_table_hash(const xcross_table_s *ct, const cube18B_xcross_s *key) {
    size_t hash = 0;
    for (uint8_t ind = 0; ind < 12; ind++) {
        hash ^= key->cubies[ind];
        hash  = rolq(hash, 3);
    }

    return hash % ct->size;
}

bool xcross_table_insert(xcross_table_s *ct, const cube18B_xcross_s *key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    size_t hash = xcross_table_hash(ct, key);
    size_t index = hash;

    // linear probing
    while (ct->table[index].algs.list != NULL) {
        if (compare_cube18B_xcross(&(ct->table[index].key), key)) {
            if (ct->table[index].algs.num_algs == ct->table[index].algs.size) {
                size_t new_size = 2*sizeof(alg_s)*ct->table[index].algs.size;
                alg_s *tmp = realloc(ct->table[index].algs.list, new_size);

                if (!tmp) {
                    return false;
                }

                ct->table[index].algs.list = tmp;
                ct->table[index].algs.size *=2;
            }

            ct->table[index].algs.list[ct->table[index].algs.num_algs] = alg_static_copy(moves);
            ct->table[index].algs.num_algs++;

            return true;
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

    ct->entries++;

    ct->table[index].algs.list = (alg_s*)malloc(sizeof(alg_s));
    ct->table[index].algs.list[0] = alg_static_copy(moves);

    ct->table[index].algs.num_algs = 1;
    ct->table[index].algs.size = 1;

    return true;
}

static inline size_t xcross_table_get_cube_index(const xcross_table_s *ct, const cube18B_xcross_s *cube) {
    if (ct == NULL || cube == NULL) {
        return ct->size;
    }

    size_t hash = xcross_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].algs.list != NULL) {
        if (compare_cube18B_xcross(&(ct->table[index].key), cube)) {
            return index;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ct->size) ? 0 : index + 1;
    }

    return ct->size;
}

const alg_list_s* xcross_table_lookup(const xcross_table_s *ct, const cube18B_xcross_s *cube) {
    size_t index = xcross_table_get_cube_index(ct, cube);

    return (index == ct->size) ? NULL : &ct->table[index].algs;
}

void xcross_table_clear(xcross_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        return;
    }

    for (size_t index = 0; index < ct->size; index++) {
        if (ct->table[index].algs.list != NULL) {
            for (size_t i = 0; i < ct->table[index].algs.num_algs; i++) {
                free(ct->table[index].algs.list[i].moves);
            }

            free(ct->table[index].algs.list);
            ct->table[index].algs.list = NULL;
        }
    }

    ct->entries = 0;
}

void xcross_table_free(xcross_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        free(ct);
        return;
    }

    xcross_table_clear(ct);

    free(ct->table);
    free(ct);
}

void xcross_table_print(xcross_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            printf("%10zu ", idx);
            print_cube18B_xcross(&(ct->table[idx].key));
            print_alg(&ct->table[idx].algs.list[0]);
        }

        if (idx != 0) {
            if (ct->table[idx].algs.list == NULL && ct->table[idx-1].algs.list != NULL) {
                printf("                                    ...                                    \n");
            }
        }
    }
}

size_t xcross_table_entries(const xcross_table_s *ct) {
    return ct->entries;
}

size_t xcross_table_size(const xcross_table_s *ct) {
    return ct->size;
}
