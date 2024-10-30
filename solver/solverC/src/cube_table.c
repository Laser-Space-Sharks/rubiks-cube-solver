#include "cube.h"
#include "cube_table.h"

typedef struct {
    cube_s *key;
    move_list_s *value;
} table_entry_s;

typedef struct cube_table {
    table_entry_s *table;
    size_t entries;
    size_t size;
} cube_table_s;

cube_table_s* cube_table_create(size_t size) {
    cube_table_s *ct = (cube_table_s*)malloc(sizeof(cube_table_s));

    ct->table = (table_entry_s*)calloc(size, sizeof(table_entry_s));
    ct->size = size;
    return ct;
}

uint32_t cube_table_hash(const cube_table_s *ct, const cube_s *cube) {
    uint32_t hash = 0;
        for (face_e face = 0; face < NUM_FACES; face++) {
            hash *= (1024 % ct->size);
            hash %= ct->size;
            hash += (cube->state[face] % ct->size);
            hash %= ct->size;
    }

    return hash;
}

bool cube_table_insert(cube_table_s *ct, const cube_s *cube, const move_list_s *moves) {
    uint32_t hash = cube_table_hash(ct, cube);

    if (ct->table[hash].value == NULL) {
        ct->table[hash].key = cube_copy(cube);
        ct->table[hash].value = move_list_copy(moves);
        ct->entries++;
        return true;
    }

    // linear probing!
    size_t index = (hash + 1) % ct->size;
    while (ct->table[index].key != NULL) {
        // we've looped through the whole hash table, and it's full
        if (index == hash) {
            return false;
        }

        if (++index >= ct->size) {
            index = 0;
        }
    }

    ct->table[index].key = cube_copy(cube);
    ct->table[index].value = move_list_copy(moves);
    ct->entries++;
    return true;
}

move_list_s* cube_table_lookup(const cube_table_s *ct, const cube_s *cube) {
    uint32_t hash = cube_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].key != NULL) {
        if (compare_cubes(*(ct->table[index].key), *cube)) {
            return ct->table[index].value;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ct->size) ? 0 : index + 1;
    }

    return NULL;
}

void cube_table_free(cube_table_s *ct) {
    for (size_t i = 0; i < ct->size; i++) {
        // we will only have non-null values if the keys are also not null
        if (ct->table[i].key != NULL) {
            free(ct->table[i].value);
            free(ct->table[i].key);
        }
    }

    free(ct->table);
    free(ct);
}
