#include "shift_cube.h"
#include "solver_print.h"
#include "cube_alg_table.h"

cube_alg_table_s* cube_alg_table_create(size_t size) {
    cube_alg_table_s *ct = (cube_alg_table_s*)malloc(sizeof(cube_alg_table_s));

    ct->table = (cube_alg_entry_s*)calloc(size, sizeof(cube_alg_entry_s));

    ct->entries = 0;
    ct->size    = size;
    return ct;
}

size_t cube_alg_table_hash(const cube_alg_table_s *ct, const shift_cube_s *key) {
    size_t hash = 0;
    for (face_e face = FACE_U; face < NUM_FACES; face++) {
        hash ^= key->state[face];
        hash  = rolq(hash, 17);
    }

    return hash % ct->size;
}
cube_alg_entry_s* cube_alg_table_get_insertion_index(const cube_alg_table_s *ct, const shift_cube_s *key) {
    size_t hash = cube_alg_table_hash(ct, key);
    size_t index = hash;

    // linear probing
    while (ct->table[index].alg.moves != NULL) {
        if (compare_cubes(&(ct->table[index].key), key)) {
            return &ct->table[index];
        }

        index++;
        if (index == hash) {
            return NULL;
        }

        if (index >= ct->size) {
            index = 0;
        }
    } return &ct->table[index];
}

bool cube_alg_table_overwrite(cube_alg_table_s *ct, const shift_cube_s *key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    cube_alg_entry_s* entry = cube_alg_table_get_insertion_index(ct, key);
    if (entry == NULL) return false;

    if (entry->alg.moves != NULL) {
        free(entry->alg.moves);
        entry->alg = alg_static_copy(moves);
        return true;
    } else {
        entry->key = *key;
        entry->alg = alg_static_copy(moves);
        ct->entries++;
        return true;
    }
}

bool cube_alg_table_overwrite_if_better(cube_alg_table_s *ct, const shift_cube_s *key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    cube_alg_entry_s* entry = cube_alg_table_get_insertion_index(ct, key);
    if (entry == NULL) return false;

    if (entry->alg.moves != NULL) {
        if (entry->alg.length <= moves->length) return false;
        free(entry->alg.moves);
        entry->alg = alg_static_copy(moves);
        return true;
    } else {
        entry->key = *key;
        entry->alg = alg_static_copy(moves);
        ct->entries++;
        return true;
    }
}

bool cube_alg_table_insert_if_new(cube_alg_table_s *ct, const shift_cube_s *key, const alg_s *moves) {
    if (ct == NULL || key == NULL || moves == NULL) {
        return false;
    }

    cube_alg_entry_s* entry = cube_alg_table_get_insertion_index(ct, key);
    if (entry == NULL) return false;

    if (entry->alg.moves != NULL) {
        return false;
    } else {
        entry->key = *key;
        entry->alg = alg_static_copy(moves);
        ct->entries++;
        return true;
    }
}

static inline cube_alg_entry_s* cube_alg_table_find_cube(const cube_alg_table_s *ct, const shift_cube_s *cube) {
    if (ct == NULL || cube == NULL) {
        return NULL;
    }

    cube_alg_entry_s* entry = cube_alg_table_get_insertion_index(ct, cube);
    if (entry == NULL) return NULL;
    if (entry->alg.moves == NULL) return NULL;
    return entry; 
}

const alg_s* cube_alg_table_lookup(const cube_alg_table_s *ct, const shift_cube_s *cube) {
    const cube_alg_entry_s* entry = cube_alg_table_find_cube(ct, cube);

    return (entry == NULL) ? NULL : &entry->alg;
}

void cube_alg_table_clear_entry(cube_alg_entry_s* entry) {
    if (entry == NULL) return;
    free(entry->alg.moves);
    entry->alg.moves = NULL;
    entry->alg.size = 0;
    entry->alg.length = 0;
    entry->key = NULL_CUBE;
}
void cube_alg_table_clear(cube_alg_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        return;
    }


    for (size_t index = 0; index < ct->size; index++) {
        if (ct->table[index].alg.moves != NULL) {
            cube_alg_table_clear_entry(&ct->table[index]);
        }
    }

    ct->entries = 0;
}

void cube_alg_table_free(cube_alg_table_s *ct) {
    if (ct == NULL || ct->table == NULL) {
        free(ct);
        return;
    }

    cube_alg_table_clear(ct);

    free(ct->table);
    free(ct);
}

void cube_alg_table_print(const cube_alg_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves != NULL) {
            printf("%10zu ", idx);
            print_cube_line_colors(ct->table[idx].key);
            print_alg(&ct->table[idx].alg);
        }

        if (idx != 0) {
            if (ct->table[idx].alg.moves == NULL && ct->table[idx-1].alg.moves != NULL) {
                printf("                                    ...                                    \n");
            }
        }
    }
}

size_t cube_alg_table_entries(const cube_alg_table_s *ct) {
    return ct->entries;
}

size_t cube_alg_table_size(const cube_alg_table_s *ct) {
    return ct->size;
}

void cube_alg_table_print_algs(const cube_alg_table_s *ct) {
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].alg.moves == NULL) continue;
        print_alg(&ct->table[idx].alg);
    }
}
