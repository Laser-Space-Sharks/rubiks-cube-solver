#include "cube18B.h"
#include "solver_print.h"
#include "xcross1_table.h"
#include "shift_cube.h"

typedef struct {
    cube18B_xcross1_s key;
    alg_list_s algs;
} xcross1_entry_s;

typedef struct xcross1_table {
    size_t entries;
    size_t size;

    xcross1_entry_s *table;
} xcross1_table_s;

xcross1_table_s* xcross1_table_create(size_t size) {
    xcross1_table_s *ct = (xcross1_table_s*)malloc(sizeof(xcross1_table_s));

    ct->table = (xcross1_entry_s*)calloc(size, sizeof(xcross1_entry_s));

    ct->entries = 0;
    ct->size    = size;
    return ct;
}

size_t xcross1_table_hash(const xcross1_table_s *ct, const cube18B_xcross1_s *key) {
    size_t hash = 0;
    for (uint8_t ind = 0; ind < 6; ind++) {
        hash ^= key->cubies[ind];
        hash  = rolq(hash, 3);
    }

    return hash % ct->size;
}

bool xcross1_table_insert(xcross1_table_s *ct, const cube18B_xcross1_s *key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }
    //if (moves->length > 5) {
    //    printf("WE INSERTED AN ALG LONGER THAN 5!!!!!  OH THE HORRORS!!!!\n");
    //}

    size_t hash = xcross1_table_hash(ct, key);
    size_t index = hash;

    // linear probing
    while (ct->table[index].algs.list != NULL) {
        if (compare_cube18B_xcross1(&(ct->table[index].key), key)) {
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
            printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA, THE TABLE RAN OUT OF SPACE!!!!!!!!!\n");
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

static inline size_t xcross1_table_get_cube_index(const xcross1_table_s *ct, const cube18B_xcross1_s *cube) {
    if (ct == NULL || cube == NULL) {
        return ct->size;
    }

    size_t hash = xcross1_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].algs.list != NULL) {
        if (compare_cube18B_xcross1(&(ct->table[index].key), cube)) {
            return index;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ct->size) ? 0 : index + 1;
    }

    return ct->size;
}

const alg_list_s* xcross1_table_lookup(const xcross1_table_s *ct, const cube18B_xcross1_s *cube) {
    size_t index = xcross1_table_get_cube_index(ct, cube);

    return (index == ct->size) ? NULL : &ct->table[index].algs;
}

void xcross1_table_clear(xcross1_table_s *ct) {
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

void xcross1_table_free(xcross1_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        free(ct);
        return;
    }

    xcross1_table_clear(ct);

    free(ct->table);
    free(ct);
}

void xcross1_table_print(xcross1_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].algs.list != NULL) {
            printf("%10zu ", idx);
            print_cube18B_xcross1(&(ct->table[idx].key));
            print_alg(&ct->table[idx].algs.list[0]);
        }

        if (idx != 0) {
            if (ct->table[idx].algs.list == NULL && ct->table[idx-1].algs.list != NULL) {
                printf("                                    ...                                    \n");
            }
        }
    }
}

size_t xcross1_table_entries(const xcross1_table_s *ct) {
    return ct->entries;
}

size_t xcross1_table_size(const xcross1_table_s *ct) {
    return ct->size;
}
