#include "servoCoder.h"
#include "inter_move_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#define INTER_MOVE_TABLE_CAPACITY 162
#define INTER_MOVE_TABLE_PATHS_PER_NODE 735

typedef struct inter_move_table {
    RSS_entry_s RSS;

    size_t size;
    inter_move_entry_s *table;
} inter_move_table_s;

inter_move_table_s* inter_move_table_create() {
    inter_move_table_s *ht = (inter_move_table_s*)malloc(sizeof(inter_move_table_s));
    ht->table = (inter_move_entry_s*)calloc(INTER_MOVE_TABLE_CAPACITY, sizeof(inter_move_entry_s));
    ht->RSS.paths = (RSS_sub_entry_s*)calloc(INTER_MOVE_TABLE_PATHS_PER_NODE, sizeof(RSS_sub_entry_s));

    ht->RSS.length = 0;
    ht->RSS.size = INTER_MOVE_TABLE_PATHS_PER_NODE;
    ht->RSS.startState = ROBOT_START_STATE;
    ht->size = INTER_MOVE_TABLE_CAPACITY;

    insert_normal_lines_into_inter_move_table(ht, "../../servoCoding/ServoOptimizationTable.txt");
    insert_root_lines_into_inter_move_table(ht, "../../servoCoding/ServoOptimizationTable_rootpaths.txt");
    return ht;
}

size_t inter_move_table_hash(const RobotState_s *key) {
    size_t hash = 0;
    hash += key->n;
    hash *= 3;
    hash += key->U;
    hash *= 3;
    hash += key->R;
    hash *= 3;
    hash += key->D;
    hash *= 3;
    hash += key->L;
    return hash;
}

bool inter_move_table_insert(inter_move_table_s *ht, const char* line) {
    if (ht == NULL || line == NULL) {
        return false;
    }
    //////////////////////////// GET ARRAY OF STATES FROM STRING OF SPACE-SEPARATED NUMBERS ////////////////////

    size_t len = strlen(line);
    while (line[len-1] == '\0' || line[len-1] == ' ' || line[len-1] == '\n') --len;

    size_t space_count = 0;
    for (size_t i = 0; i < len; ++i) space_count += (line[i] == ' ');

    uint8_t stateNumStrLengths[space_count+1];
    size_t index = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') ++index;
        else stateNumStrLengths[index]++;
    }
    char* arr_of_stateNumStrs[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNumStrs[i] = (char*)calloc(sizeof(char), stateNumStrLengths[i]+1);
    }
    index = 0;
    size_t last_set = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') last_set = ++index;
        else arr_of_stateNumStrs[index][i-last_set] = line[i];
    }
    uint16_t arr_of_stateNums[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNums[i] = (uint16_t)atoi(arr_of_stateNumStrs[i]);
        free(arr_of_stateNumStrs[i]);
    }

    State_s arr_of_states[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_states[i] = stateNum_to_state(arr_of_stateNums[i]);
    }
    double total_weight = 0;
    for (int i = 0; i < space_count; ++i) {
        total_weight += calc_weight_of_step(&arr_of_states[i], &arr_of_states[i+1]);
    }

    ///////////////////////////////////// INSERT NEW PATH ONTO NODE //////////////////////////////

    size_t index = inter_move_table_hash(&(arr_of_states[0].servos));

    if (ht->table[index].paths == NULL) {
        ht->table[index].startState = arr_of_states[0].servos;
        ht->table[index].paths = (sub_entry_s*)malloc(INTER_MOVE_TABLE_PATHS_PER_NODE*sizeof(sub_entry_s));
        ht->table[index].length = 0;
        ht->table[index].size = INTER_MOVE_TABLE_PATHS_PER_NODE;
    }

    sub_entry_s sub_entry = {
        .endState = arr_of_states[space_count],
        .weight = total_weight,
        .path = (RobotState_s*)malloc(sizeof(RobotState_s)*(space_count-1)),
        .size = space_count-1
    }; 
    for (int i = 1; i < space_count; ++i) {
        sub_entry.path[i-1] = arr_of_states[i].servos;
    }

    ht->table[index].startState  = arr_of_states[0].servos;
    ht->table[index].paths[ht->table[index].length]    = sub_entry;

    ht->table[index].length++;

    return true;
}
bool inter_move_table_RSS_insert(inter_move_table_s *ht, const char* line) {
    if (ht == NULL || line == NULL) {
        return false;
    }
    //////////////////////////// GET ARRAY OF STATES FROM STRING OF SPACE-SEPARATED NUMBERS ////////////////////
    
    size_t len = strlen(line);
     while (line[len-1] == '\0' || line[len-1] == ' ' || line[len-1] == '\n') --len;

    size_t space_count = 0;
    for (size_t i = 0; i < len; ++i) space_count += (line[i] == ' ');

    uint8_t stateNumStrLengths[space_count+1];
    size_t index = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') ++index;
        else stateNumStrLengths[index]++;
    }
    char* arr_of_stateNumStrs[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNumStrs[i] = (char*)calloc(sizeof(char), stateNumStrLengths[i]+1);
    }
    index = 0;
    size_t last_set = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') last_set = ++index;
        else arr_of_stateNumStrs[index][i-last_set] = line[i];
    }
    uint16_t arr_of_stateNums[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNums[i] = (uint16_t)atoi(arr_of_stateNumStrs[i]);
        free(arr_of_stateNumStrs[i]);
    }

    State_s arr_of_states[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_states[i] = stateNum_to_state(arr_of_stateNums[i]);
    }

    double total_weight = 0;
    for (int i = 0; i < space_count; ++i) {
        total_weight += calc_weight_of_step(&arr_of_states[i], &arr_of_states[i+1]);
    }

    ///////////////////////////////////// INSERT NEW PATH ONTO NODE //////////////////////////////

    ht->RSS.paths[ht->RSS.length] = (RSS_sub_entry_s) {
        .endState = arr_of_states[space_count],
        .weight = total_weight,
        .path = (State_s*)malloc(sizeof(State_s)*(space_count-1)),
        .size = space_count-1
    };
    for (int i = 1; i < space_count; ++i) {
        ht->RSS.paths[ht->RSS.length].path[i-1] = arr_of_states[i];
    } ht->RSS.length++;

    return true;
}
void insert_normal_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t len;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        inter_move_table_insert(INTER_MOVE_TABLE, line);
    }

    free(line);
    fclose(file);
}

void insert_root_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t len;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        inter_move_table_RSS_insert(INTER_MOVE_TABLE, line);
    }

    free(line);
    fclose(file);
}

static inline size_t inter_move_table_get_index(const inter_move_table_s *ht, const RobotState_s *key) {
    if (ht == NULL || key == NULL) {
        return ht->size;
    }

    size_t hash = inter_move_table_hash(key);
    size_t index = hash;

    while (ht->table[index].paths != NULL) {
        if (compare_RobotStates(&(ht->table[index].startState), key)) {
            return index;
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= ht->size) ? 0 : index + 1;
    }

    return ht->size;
}

const RSS_entry_s* inter_move_table_get_RSS(const inter_move_table_s *ht) {
    return &(ht->RSS);
}

const inter_move_entry_s* inter_move_table_lookup(const inter_move_table_s *ht, const RobotState_s *key) {
    size_t index = inter_move_table_get_index(ht, key);

    return (index == ht->size) ? NULL : &ht->table[index];
}

void inter_move_table_free(inter_move_table_s *ht) {
    if (ht == NULL || ht->table == NULL) {
        free(ht);
        return;
    }
    for (size_t subEntryInd = 0; subEntryInd < INTER_MOVE_TABLE_PATHS_PER_NODE; subEntryInd++) {
        free(ht->RSS.paths[subEntryInd].path);
    } free(ht->RSS.paths);
    for (size_t entryInd = 0; entryInd < INTER_MOVE_TABLE_CAPACITY; entryInd++) {
        for (size_t subEntryInd = 0; subEntryInd < INTER_MOVE_TABLE_PATHS_PER_NODE; subEntryInd++) {
            free(ht->table[entryInd].paths[subEntryInd].path);
        } free(ht->table[entryInd].paths);
    } free(ht->table);
    free(ht);
}