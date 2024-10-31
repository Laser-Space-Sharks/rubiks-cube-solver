#include "cube.h"
#include "solver_print.h"
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
    ct->entries = 0;
    ct->size = size;
    return ct;
}

size_t cube_table_hash(const cube_table_s *ct, const cube_s *cube) {
    size_t hash = 0;
        for (face_e face = 0; face < NUM_FACES; face++) {
            hash *= (1024 % ct->size);
            hash %= ct->size;
            hash += (cube->state[face] % ct->size);
            hash %= ct->size;
    }

    return hash;
}

bool cube_table_insert(cube_table_s *ct, const cube_s *cube, const move_list_s *moves) {
    size_t hash = cube_table_hash(ct, cube);
    size_t index = hash;

    // first check if the cube is in here to begin with
    if (cube_table_lookup(ct, cube) != NULL) {
        return false;
    }

    // linear probing
    while (ct->table[index].key != NULL) {
        index++;
        if (index == hash) {
            return false;
        }

        if (index >= ct->size) {
            index = 0;
        }
    }

    ct->table[index].key = cube_copy(cube);
    ct->table[index].value = move_list_copy(moves);
    ct->entries++;
    return true;
}

move_list_s* cube_table_lookup(const cube_table_s *ct, const cube_s *cube) {
    size_t hash = cube_table_hash(ct, cube);
    size_t index = hash;

    while (ct->table[index].key != NULL) {
        if (compare_cubes(ct->table[index].key, cube)) {
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
            move_list_free(ct->table[i].value);
            free(ct->table[i].key);
        }
    }

    free(ct->table);
    free(ct);
}

void cube_table_print(cube_table_s *ct) {
    printf("   index  |               cube string representation            | algorithm\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t idx = 0; idx < ct->size; idx++) {
        if (ct->table[idx].key != NULL) {
            printf("%10zu ", idx);
            print_cube_line_colors(*(ct->table[idx].key));
            print_move_list(ct->table[idx].value);
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
