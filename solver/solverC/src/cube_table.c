#include "cube.h"
#include "solver_print.h"
#include "cube_table.h"

typedef struct {
    cube_s *key;
    alg_list_s *algs;
} entry_s;

typedef struct cube_table {
    size_t entries;
    size_t size;

    entry_s *table;
    cube_s *keys;
} cube_table_s;

cube_table_s* cube_table_create(size_t size) {
    cube_table_s *ct = (cube_table_s*)malloc(sizeof(cube_table_s));

    ct->table = (entry_s*)calloc(size, sizeof(entry_s));
    ct->keys  = (cube_s*)malloc(sizeof(cube_s) * size);

    ct->entries = 0;
    ct->size    = size;
    return ct;
}

size_t cube_table_hash(const cube_table_s *ct, const cube_s *key) {
    size_t hash = 0;
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        hash ^= key->state[face];
        hash  = rolq(hash, 17);
    }

    return hash % ct->size;
}

bool cube_table_insert(cube_table_s *ct, const cube_s *key, const move_list_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    size_t hash = cube_table_hash(ct, key);
    size_t index = hash;

    // linear probing
    while (ct->table[index].key != NULL) {
        if (compare_cubes(ct->table[index].key, key)) {
            if (ct->table[index].algs->num_algs == ct->table[index].algs->size) {
                size_t new_size = 2*sizeof(move_list_s)*ct->table[index].algs->size;
                move_list_s *tmp = realloc(ct->table[index].algs->list, new_size);

                if (!tmp) {
                    return false;
                }

                ct->table[index].algs->list = tmp;
                ct->table[index].algs->size *=2;
            }

            ct->table[index].algs->list[ct->table[index].algs->num_algs] = move_list_static_copy(moves);
            ct->table[index].algs->num_algs++;

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


    ct->keys[ct->entries] = *key;
    ct->table[index].key  = &(ct->keys[ct->entries]);

    ct->entries++;

    ct->table[index].algs = (alg_list_s*)malloc(sizeof(alg_list_s));
    ct->table[index].algs->list = (move_list_s*)malloc(sizeof(move_list_s));
    ct->table[index].algs->list[0] = move_list_static_copy(moves);

    ct->table[index].algs->num_algs = 1;
    ct->table[index].algs->size = 1;

    return true;
}

static inline size_t cube_table_get_cube_index(const cube_table_s *ct, const cube_s *cube) {
    if (ct == NULL || cube == NULL) {
        return ct->size;
    }

    size_t hash = cube_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].key != NULL) {
        if (compare_cubes(ct->table[index].key, cube)) {
            return index;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ct->size) ? 0 : index + 1;
    }

    return ct->size;
}

const alg_list_s* cube_table_lookup(const cube_table_s *ct, const cube_s *cube) {
    size_t index = cube_table_get_cube_index(ct, cube);

    return (index == ct->size) ? NULL : ct->table[index].algs;
}

void cube_table_clear(cube_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        return;
    }

    for (size_t i = 0; i < ct->entries; i++) {
        size_t index = cube_table_get_cube_index(ct, &ct->keys[i]);
        while (index < ct->size && ct->table[index].key != NULL) {
            for (size_t j = 0; j < ct->table[index].algs->num_algs; j++) {
                free(ct->table[index].algs->list[j].list);
            }
            free(ct->table[index].algs->list);
            free(ct->table[index].algs);
            ct->table[index].key = NULL;
            ct->table[index].algs = NULL;

            index++;
        }

    }

    ct->entries = 0;
}

void cube_table_free(cube_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        free(ct);
        return;
    }

    for (size_t i = 0; i < ct->entries; i++) {
        size_t index = cube_table_get_cube_index(ct, &ct->keys[i]);
        for (size_t j = 0; j < ct->table[index].algs->num_algs; j++) {
            free(ct->table[index].algs->list[j].list);
        }

        free(ct->table[index].algs->list);
        free(ct->table[index].algs);
    }

    free(ct->table);
    free(ct->keys);
    free(ct);
}

void cube_table_print(cube_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].key != NULL) {
            printf("%10zu ", idx);
            print_cube_line_colors(*ct->table[idx].key);
            print_move_list(&ct->table[idx].algs->list[0]);
        }

        if (idx != 0) {
            if (ct->table[idx].key == NULL && ct->table[idx-1].key != NULL) {
                printf("                                    ...                                    \n");
            }
        }
    }
}

size_t cube_table_entries(const cube_table_s *ct) {
    return ct->entries;
}

size_t cube_table_size(const cube_table_s *ct) {
    return ct->size;
}
