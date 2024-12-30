#define _GNU_SOURCE
#include "servoCoder.h"
#include "main.h"
#include "alg.h"
#include "move.h"
#include "MinHeap.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "solver_print.h"

#define INTER_MOVE_TABLE_CAPACITY 162
#define INTER_MOVE_TABLE_PATHS_PER_NODE 735

#define Etime 1
#define dEtime 1
#define rot1time 1
#define rot2time 1

void print_RobotState(RobotState_s servos) {
    printf("%c.%c.%c.%c.U%hhu.R%hhu.D%hhu.L%hhu",
        ((servos.n) ? 'N' : 'n'),
        ((servos.e) ? 'E' : 'e'),
        ((servos.s) ? 'S' : 's'),
        ((servos.w) ? 'W' : 'w'),
        servos.U,
        servos.R,
        servos.D,
        servos.L
    );
}
void print_State(State_s state) {
    printf("((%c, %hhu), ", 
        facePrints[state.persp.face], 
        state.persp.rot
    );
    print_RobotState(state.servos);
    printf(")");
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
    //printf("----------------------------\nLine was : %s", line);
    //////////////////////////// GET ARRAY OF STATES FROM STRING OF SPACE-SEPARATED NUMBERS ////////////////////

    size_t len = 0; while(line[len]) len++;
    while (len-1 >= 0 && (line[len-1] == '\0' || line[len-1] == ' ' || line[len-1] == '\n')) --len;
    //printf("Decided that len was %zu\n", len);

    size_t space_count = 0;
    for (size_t i = 0; i < len; ++i) space_count += (line[i] == ' ');
    //printf("Decided that space_count was %zu\n", space_count);

    uint8_t stateNumStrLengths[space_count+1];
    for (int i = 0; i < space_count+1; i++) stateNumStrLengths[i] = 0;
    //printf("Decided that there were %zu numbers in the string\n", space_count+1);
    size_t index = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') index++;
        else stateNumStrLengths[index]++;
    }
    //printf("Decided that the lengths of the numstrs were: ");
    //for (int i = 0; i < space_count+1; i++) printf("%hhu, ", stateNumStrLengths[i]);
    //printf("\n");
    char* arr_of_stateNumStrs[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNumStrs[i] = (char*)calloc(sizeof(char), stateNumStrLengths[i]+1);
    }
    index = 0;
    size_t last_set = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') {index++; last_set = i+1;}
        else {
            arr_of_stateNumStrs[index][i-last_set] = line[i];
        }
    }
    //printf("Decided that the numstrs were: ");
    //for (int i = 0; i < space_count+1; i++) printf("%s, ", arr_of_stateNumStrs[i]);
    //printf("\n");
    uint16_t arr_of_stateNums[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNums[i] = (uint16_t)atoi(arr_of_stateNumStrs[i]);
        free(arr_of_stateNumStrs[i]);
    }

    State_s arr_of_states[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_states[i] = stateNum_to_state(arr_of_stateNums[i]);
    }
    float total_weight = 0;
    for (int i = 0; i < space_count; ++i) {
        total_weight += calc_weight_of_step(&arr_of_states[i], &arr_of_states[i+1]);
    }

    ///////////////////////////////////// INSERT NEW PATH ONTO NODE //////////////////////////////

    index = inter_move_table_hash(&(arr_of_states[0].servos));

    if (ht->table[index].paths == NULL) {
        ht->table[index].startState = arr_of_states[0].servos;
        ht->table[index].paths = (sub_entry_s*)malloc(INTER_MOVE_TABLE_PATHS_PER_NODE*sizeof(sub_entry_s));
        ht->table[index].length = 0;
        ht->table[index].size = INTER_MOVE_TABLE_PATHS_PER_NODE;
    }

    sub_entry_s sub_entry = {
        .endState = arr_of_states[space_count],
        .singleMoveQualifications = 0,
        .weight = total_weight,
        .path = (RobotState_s*)malloc(sizeof(RobotState_s)*(space_count-1)),
        .size = space_count-1
    };
    for (int i = 1; i < space_count; ++i) {
        sub_entry.path[i-1] = arr_of_states[i].servos;
    }
    for (move_e move = 0; move < 18; move++) {
        move_s movestruct = move_as_struct[move];
        if (state_can_do_move(movestruct, sub_entry.endState)) {
            sub_entry.singleMoveQualifications |= (1<<move);
        }
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
    //printf("----------------------------\nLine was : %s", line);
    //////////////////////////// GET ARRAY OF STATES FROM STRING OF SPACE-SEPARATED NUMBERS ////////////////////

    size_t len = 0; while(line[len]) len++;
    while (len-1 >= 0 && (line[len-1] == '\0' || line[len-1] == ' ' || line[len-1] == '\n')) --len;
    //printf("Decided that len was %zu\n", len);

    size_t space_count = 0;
    for (size_t i = 0; i < len; ++i) space_count += (line[i] == ' ');
    //printf("Decided that space_count was %zu\n", space_count);

    uint8_t stateNumStrLengths[space_count+1];
    for (int i = 0; i < space_count+1; i++) stateNumStrLengths[i] = 0;
    //printf("Decided that there were %zu numbers in the string\n", space_count+1);
    size_t index = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') index++;
        else stateNumStrLengths[index]++;
    }
    //printf("Decided that the lengths of the numstrs were: ");
    //for (int i = 0; i < space_count+1; i++) printf("%hhu, ", stateNumStrLengths[i]);
    //printf("\n");
    char* arr_of_stateNumStrs[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNumStrs[i] = (char*)calloc(sizeof(char), stateNumStrLengths[i]+1);
    }
    index = 0;
    size_t last_set = 0;
    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') {index++; last_set = i+1;}
        else {
            arr_of_stateNumStrs[index][i-last_set] = line[i];
        }
    }
    //printf("Decided that the numstrs were: ");
    //for (int i = 0; i < space_count+1; i++) printf("%s, ", arr_of_stateNumStrs[i]);
    //printf("\n");
    uint16_t arr_of_stateNums[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_stateNums[i] = (uint16_t)atoi(arr_of_stateNumStrs[i]);
        free(arr_of_stateNumStrs[i]);
    }

    State_s arr_of_states[space_count+1];
    for (int i = 0; i < space_count+1; ++i) {
        arr_of_states[i] = stateNum_to_state(arr_of_stateNums[i]);
    }
    float total_weight = 0;
    for (int i = 0; i < space_count; ++i) {
        total_weight += calc_weight_of_step(&arr_of_states[i], &arr_of_states[i+1]);
    }

    ///////////////////////////////////// INSERT NEW PATH ONTO NODE //////////////////////////////
    ht->RSS.paths[ht->RSS.length] = (RSS_sub_entry_s) {
        .endState = arr_of_states[space_count],
        .singleMoveQualifications = 0,
        .weight = total_weight,
        .path = (State_s*)malloc(sizeof(State_s)*(space_count-1)),
        .size = space_count-1
    };
    for (move_e move = 0; move < 18; move++) {
        move_s movestruct = move_as_struct[move];
        if (state_can_do_move(movestruct, ht->RSS.paths[ht->RSS.length].endState)) {
            ht->RSS.paths[ht->RSS.length].singleMoveQualifications |= (1<<move);
        }
    }
    for (int i = 1; i < space_count; ++i) {
        ht->RSS.paths[ht->RSS.length].path[i-1] = arr_of_states[i];
    } ht->RSS.length++;

    return true;
}
void insert_normal_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, char *filename) {
    //printf("starting 'insert_normal_lines_into_inter_move_table'\n");
    FILE *file = fopen(filename, "rb");
    //printf("opened the file...\n");
    if (file == NULL) return;

    char *line = NULL;
    size_t len;
    ssize_t read;

    //printf("inserting lines...\n");

    while ((read = getline(&line, &len, file)) != -1) {
        inter_move_table_insert(INTER_MOVE_TABLE, line);
    }
    //printf("finished inserting lines...\n");

    free(line);
    fclose(file);
    //printf("finished 'insert_normal_lines_into_inter_move_table'\n");
}

void insert_root_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, char *filename) {
    //printf("starting 'insert_root_lines_into_inter_move_table'\n");
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return;

    char *line = NULL;
    size_t len;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        inter_move_table_RSS_insert(INTER_MOVE_TABLE, line);
    }

    free(line);
    fclose(file);
    //printf("finished 'insert_root_lines_into_inter_move_table'\n");
}

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

    size_t numSubEntries = 0;
    size_t numRSSSubEntries = 0;
    size_t numInterPathNodes = 0;
    size_t numRSSInterPathNodes = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i].length == 0) continue;
        numSubEntries += ht->table[i].length;
        for (int j = 0; j < ht->table[i].length; j++) {
            numInterPathNodes += ht->table[i].paths[j].size;
        }
    }
    numRSSSubEntries += ht->RSS.length;
    for (int j = 0; j < ht->RSS.length; j++) {
        numRSSInterPathNodes += ht->RSS.paths[j].size;
    }
    printf("In the INTER_MOVE_TABLE, there are %zu SubEntries, %zu RSSSubEntries, %zu InterPathNodes, %zu RSSInterPathNodes", 
        numSubEntries,
        numRSSSubEntries,
        numInterPathNodes,
        numRSSInterPathNodes
    );


    //printf("RSS.length: %zu\n", ht->RSS.length);
    return ht;
}

static inline size_t inter_move_table_get_index(const inter_move_table_s *ht, const RobotState_s *key) {
    if (ht == NULL || key == NULL) {
        return ht->size;
    }

    size_t hash = inter_move_table_hash(key);
    size_t index = hash;

    if (ht->table[index].paths == NULL) return ht->size;

    return index;
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
    //printf("freeing RSS.paths..\n");
    for (size_t subEntryInd = 0; subEntryInd < ht->RSS.length; subEntryInd++) {
        free(ht->RSS.paths[subEntryInd].path);
    } free(ht->RSS.paths);
    //printf("freeing table stuff..\n");
    for (size_t entryInd = 0; entryInd < ht->size; entryInd++) {
        for (size_t subEntryInd = 0; subEntryInd < ht->table[entryInd].length; subEntryInd++) {
            free(ht->table[entryInd].paths[subEntryInd].path);
        } free(ht->table[entryInd].paths);
    } //printf("freeing table..\n");
    free(ht->table);
    //printf("freeing ht..\n");
    free(ht);
}

typedef struct {
    face_e faces[6];
} Orientation_arr6_s; // 6 Bytes

/*
ROT_TRAINS: dict[str, tuple[tuple[str, int]]] = {
    'U': (('B', 2), ('R', 1), ('F', 0), ('L', 3)),
    'R': (('U', 3), ('B', 0), ('D', 1), ('F', 0)),
    'F': (('U', 0), ('R', 0), ('D', 0), ('L', 0)),
    'L': (('U', 1), ('F', 0), ('D', 3), ('B', 0)),
    'B': (('U', 2), ('L', 0), ('D', 2), ('R', 0)),
    'D': (('F', 0), ('R', 3), ('B', 2), ('L', 1)),
}
*/

static const uint8_t multiplied_orientations[24][24] = {
	{18, 19, 16, 17, 5, 6, 7, 4, 0, 1, 2, 3, 15, 12, 13, 14, 22, 23, 20, 21, 8, 9, 10, 11},
	{6, 7, 4, 5, 9, 10, 11, 8, 1, 2, 3, 0, 19, 16, 17, 18, 21, 22, 23, 20, 12, 13, 14, 15},
	{10, 11, 8, 9, 13, 14, 15, 12, 2, 3, 0, 1, 7, 4, 5, 6, 20, 21, 22, 23, 16, 17, 18, 19},
	{14, 15, 12, 13, 17, 18, 19, 16, 3, 0, 1, 2, 11, 8, 9, 10, 23, 20, 21, 22, 4, 5, 6, 7},
	{3, 0, 1, 2, 16, 17, 18, 19, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 21, 22, 23, 20},
	{17, 18, 19, 16, 22, 23, 20, 21, 5, 6, 7, 4, 0, 1, 2, 3, 15, 12, 13, 14, 9, 10, 11, 8},
	{23, 20, 21, 22, 10, 11, 8, 9, 6, 7, 4, 5, 18, 19, 16, 17, 14, 15, 12, 13, 1, 2, 3, 0},
	{11, 8, 9, 10, 2, 3, 0, 1, 7, 4, 5, 6, 20, 21, 22, 23, 13, 14, 15, 12, 19, 16, 17, 18},
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
	{5, 6, 7, 4, 21, 22, 23, 20, 9, 10, 11, 8, 1, 2, 3, 0, 19, 16, 17, 18, 13, 14, 15, 12},
	{22, 23, 20, 21, 14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 18, 19, 16, 17, 2, 3, 0, 1},
	{15, 12, 13, 14, 3, 0, 1, 2, 11, 8, 9, 10, 23, 20, 21, 22, 17, 18, 19, 16, 7, 4, 5, 6},
	{1, 2, 3, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 4, 5, 6, 7, 23, 20, 21, 22},
	{9, 10, 11, 8, 20, 21, 22, 23, 13, 14, 15, 12, 2, 3, 0, 1, 7, 4, 5, 6, 17, 18, 19, 16},
	{21, 22, 23, 20, 18, 19, 16, 17, 14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 3, 0, 1, 2},
	{19, 16, 17, 18, 0, 1, 2, 3, 15, 12, 13, 14, 22, 23, 20, 21, 5, 6, 7, 4, 11, 8, 9, 10},
	{2, 3, 0, 1, 12, 13, 14, 15, 16, 17, 18, 19, 4, 5, 6, 7, 8, 9, 10, 11, 22, 23, 20, 21},
	{13, 14, 15, 12, 23, 20, 21, 22, 17, 18, 19, 16, 3, 0, 1, 2, 11, 8, 9, 10, 5, 6, 7, 4},
	{20, 21, 22, 23, 6, 7, 4, 5, 18, 19, 16, 17, 14, 15, 12, 13, 10, 11, 8, 9, 0, 1, 2, 3},
	{7, 4, 5, 6, 1, 2, 3, 0, 19, 16, 17, 18, 21, 22, 23, 20, 9, 10, 11, 8, 15, 12, 13, 14},
	{8, 9, 10, 11, 7, 4, 5, 6, 20, 21, 22, 23, 13, 14, 15, 12, 2, 3, 0, 1, 18, 19, 16, 17},
	{4, 5, 6, 7, 19, 16, 17, 18, 21, 22, 23, 20, 9, 10, 11, 8, 1, 2, 3, 0, 14, 15, 12, 13},
	{16, 17, 18, 19, 15, 12, 13, 14, 22, 23, 20, 21, 5, 6, 7, 4, 0, 1, 2, 3, 10, 11, 8, 9},
	{12, 13, 14, 15, 11, 8, 9, 10, 23, 20, 21, 22, 17, 18, 19, 16, 3, 0, 1, 2, 6, 7, 4, 5}
};

static const face_e ROT_TRAINS[6][4] = {
    {FACE_B, FACE_R, FACE_F, FACE_L},
    {FACE_U, FACE_B, FACE_D, FACE_F},
    {FACE_U, FACE_R, FACE_D, FACE_L},
    {FACE_U, FACE_F, FACE_D, FACE_B},
    {FACE_U, FACE_L, FACE_D, FACE_R},
    {FACE_F, FACE_R, FACE_B, FACE_L},
};

Orientation_arr6_s Arr6_from_Orientation(Orientation_s O) {
    Orientation_arr6_s wheres06;
    wheres06.faces[O.face] = FACE_F;
    wheres06.faces[opposite_faces[O.face]] = FACE_B;
    wheres06.faces[ROT_TRAINS[O.face][mod4(0 + O.rot)]] = FACE_U;
    wheres06.faces[ROT_TRAINS[O.face][mod4(1 + O.rot)]] = FACE_R;
    wheres06.faces[ROT_TRAINS[O.face][mod4(2 + O.rot)]] = FACE_D;
    wheres06.faces[ROT_TRAINS[O.face][mod4(3 + O.rot)]] = FACE_L;
    return wheres06;
}
Orientation_s Orientation_from_Arr6(Orientation_arr6_s arr6) {
    Orientation_s O;
    for (face_e pos = 0; pos < 6; pos++) {
        if (arr6.faces[pos] == FACE_F) {
            O.face = pos;
            break;
        }
    }
    for (uint8_t rot = 0; rot < 4; rot++) {
        if (arr6.faces[ROT_TRAINS[O.face][rot]] == FACE_U) {
            O.rot = rot;
            break;
        }
    }
    return O;
}

Orientation_arr6_s multiply_arr6s(Orientation_arr6_s arr1, Orientation_arr6_s arr2) {
    return (Orientation_arr6_s) {
        .faces = {
            arr2.faces[arr1.faces[0]],
            arr2.faces[arr1.faces[1]],
            arr2.faces[arr1.faces[2]],
            arr2.faces[arr1.faces[3]],
            arr2.faces[arr1.faces[4]],
            arr2.faces[arr1.faces[5]],
        }
    };
}

State_s Undefault_EndState(State_s origin, State_s OGendState) {
    //printf("3\n");
    State_s true_endState1 = OGendState;
    uint8_t a = multiplied_orientations[origin.persp.face*4 + origin.persp.rot][OGendState.persp.face*4 + OGendState.persp.rot];
    true_endState1.persp.face = a>>2;
    true_endState1.persp.rot = a&3;
    //printf("4\n");
    return true_endState1;
}

State_s stateNum_to_state(uint16_t stateNum) {
    uint8_t L = stateNum%3; stateNum /= 3;
    uint8_t D = stateNum%3; stateNum /= 3;
    uint8_t R = stateNum%3; stateNum /= 3;
    uint8_t U = stateNum%3; stateNum /= 3;
    uint8_t w = stateNum%2; stateNum /= 2;
    uint8_t s = stateNum%2; stateNum /= 2;
    uint8_t e = stateNum%2; stateNum /= 2;
    uint8_t n = stateNum%2; stateNum /= 2;
    uint8_t rot = stateNum%4; stateNum /= 4;
    face_e face = stateNum%6; stateNum /= 6;

    return (State_s) {
        .persp = (Orientation_s) {
            .face = face,
            .rot = rot
        },
        .servos = (RobotState_s) {
            .n = n,
            .e = e,
            .s = s,
            .w = w,
            .U = U,
            .R = R,
            .D = D,
            .L = L
        }
    };    
}

bool compare_RobotStates(const RobotState_s* state1, const RobotState_s* state2) {
    return (
        state1->n == state2->n &&
        state1->e == state2->n &&
        state1->s == state2->n &&
        state1->w == state2->w &&
        state1->U == state2->U &&
        state1->R == state2->R &&
        state1->D == state2->D &&
        state1->L == state2->L
    );
}

bool State_is_ROBOT_START_STATE(const State_s* state) {
    return (
        compare_RobotStates(&(state->servos), &(ROBOT_START_STATE.servos)) &&
        state->persp.face == ROBOT_START_STATE.persp.face &&
        state->persp.rot == ROBOT_START_STATE.persp.rot
    );
}
bool compare_states(const State_s* state1, const State_s* state2) {
    return (
        compare_RobotStates(&(state1->servos), &(state2->servos)) &&
        state1->persp.face == state2->persp.face &&
        state1->persp.rot == state2->persp.rot
    );
}


/*
def calc_weight_of_step(state: State, state2: State) -> float:
    MAX = 0
    for v1, v2 in zip(state.unpackServos(), state2.unpackServos()):
        if (v1.e, v2.e) == (0, 1): MAX = max(MAX, Etime)
        elif (v1.e, v2.e) == (1, 0): MAX = max(MAX, dEtime)
        elif abs(v1.rot - v2.rot) == 1: MAX = max(MAX, rot1time)
        elif abs(v1.rot - v2.rot) == 2: MAX = max(MAX, rot2time)
    return MAX
*/
float calc_weight_of_armstep(bool e1, uint8_t rot1, bool e2, uint8_t rot2) {
    float ret;
    if (e1 == 0 && e2 == 1) ret = (float)Etime;
    if (e1 == 1 && e2 == 0) ret = (float)dEtime;
    if (abs(rot2 - rot1) == 1) ret = (float)rot1time;
    if (abs(rot2 - rot1) == 2) ret = (float)rot2time;
    return ret;
}
float calc_weight_of_step(const State_s* state1, const State_s* state2) {
    float maxWeight = 0;
    float arm1 = calc_weight_of_armstep(state1->servos.n, state1->servos.U, state2->servos.n, state2->servos.U);
    float arm2 = calc_weight_of_armstep(state1->servos.e, state1->servos.R, state2->servos.e, state2->servos.R);
    float arm3 = calc_weight_of_armstep(state1->servos.s, state1->servos.D, state2->servos.s, state2->servos.D);
    float arm4 = calc_weight_of_armstep(state1->servos.w, state1->servos.L, state2->servos.w, state2->servos.L);
    maxWeight = (maxWeight > arm1) ? maxWeight : arm1;
    maxWeight = (maxWeight > arm2) ? maxWeight : arm2;
    maxWeight = (maxWeight > arm3) ? maxWeight : arm3;
    maxWeight = (maxWeight > arm4) ? maxWeight : arm4;
    return maxWeight;
}

/*
def state_can_do_move(move: Move, state: State):
    face = Orientation_to_arr6[state.persp][faces_to_nums[move.face]]
    if (face == 'F' or face == 'B'): return False
    if face == 'U':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
        if state.servos.R.rot == 1 or state.servos.L.rot == 1: return False
        if state.servos.U.rot == 3-move.turns: return False
    if face == 'R':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
        if state.servos.U.rot == 1 or state.servos.D.rot == 1: return False
        if state.servos.R.rot == 3-move.turns: return False
    if face == 'L':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
        if state.servos.U.rot == 1 or state.servos.D.rot == 1: return False
        if state.servos.L.rot == 3-move.turns: return False
    if face == 'D':
        if not (state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
        if state.servos.R.rot == 1 or state.servos.L.rot == 1: return False
        if state.servos.D.rot == 3-move.turns: return False
    return True
*/
static inline bool allequal5(bool a1, bool a2, bool a3, bool a4, bool a5) {
    return (
        a1 == a2 && a2 == a3 && a3 == a4 && a4 == a5
    );
}
static inline bool allequal4(bool a1, bool a2, bool a3, bool a4) {
    return (
        a1 == a2 && a2 == a3 && a3 == a4
    );
}
bool state_can_do_move(move_s move, State_s state) {
    face_e face = Arr6_from_Orientation(state.persp).faces[move.face];
    if (face == FACE_F || face == FACE_B) return false;
    RobotState_s servos = state.servos;
    return (
        ((face == FACE_U) && (allequal5(servos.n, servos.e, servos.s, servos.w, 1)) && (servos.R != 1 && servos.L != 1) && (servos.U != 3-move.turns))
    ||  ((face == FACE_R) && ((allequal5(servos.n, servos.e, servos.s, servos.w, 1)) && (servos.U != 1 && servos.D != 1) && (servos.R != 3-move.turns)))
    ||  ((face == FACE_D) && ((allequal4(servos.e, servos.s, servos.w, 1)) && (servos.R != 1 && servos.L != 1) && (servos.D != 3-move.turns)))
    ||  ((face == FACE_L) && ((allequal5(servos.n, servos.e, servos.s, servos.w, 1)) && (servos.U != 1 && servos.D != 1) && (servos.L != 3-move.turns)))
    );
}
/*
def state_after_move(move: Move, state: State) -> list[State]|None:
    face = Orientation_to_arr6[state.persp][faces_to_nums[move.face]]

    if move.turns == 1: ArmAfterMove = lambda rot: ArmState(1, rot+1)
    if move.turns == 2: ArmAfterMove = lambda rot: ArmState(1, 2-rot)
    if move.turns == 3: ArmAfterMove = lambda rot: ArmState(1, rot-1)
    
    if face == 'U':
        endStateOfU = ArmAfterMove(state.servos.U.rot)
        return [State(state.persp, RobotState(endStateOfU, state.servos.R, state.servos.D, state.servos.L))]
    if face == 'R':
        endStateOfR = ArmAfterMove(state.servos.R.rot)
        return [State(state.persp, RobotState(state.servos.U, endStateOfR, state.servos.D, state.servos.L))]
    if face == 'L':
        endStateOfL = ArmAfterMove(state.servos.L.rot)
        return [State(state.persp, RobotState(state.servos.U, state.servos.R, state.servos.D, endStateOfL))]
    if face == 'D':
        endStateOfD = ArmAfterMove(state.servos.D.rot)
        
        if (state.servos.U.e):
            return [State(state.persp, RobotState(state.servos.U, state.servos.R, endStateOfD, state.servos.L)),
                    State(state.persp, RobotState(ArmState(0, state.servos.U.rot), state.servos.R, endStateOfD, state.servos.L))]
        return [State(state.persp, RobotState(ArmState(1, state.servos.U.rot), state.servos.R, endStateOfD, state.servos.L)),
                State(state.persp, RobotState(ArmState(0, 0), state.servos.R, endStateOfD, state.servos.L)),
                State(state.persp, RobotState(ArmState(0, 1), state.servos.R, endStateOfD, state.servos.L)),
                State(state.persp, RobotState(ArmState(0, 2), state.servos.R, endStateOfD, state.servos.L))]
*/
void state_after_move(move_s move, State_s state, uint8_t *len, State_s* ret) {
    face_e face = Arr6_from_Orientation(state.persp).faces[move.face];
    RobotState_s servos = state.servos;
    uint8_t RotAfterMove[3][3] = {
        {1, 2, 4},
        {2, 4, 0},
        {4, 0, 1}
    };
    if (face == FACE_U) {
        uint8_t nextU = RotAfterMove[move.turns-1][servos.U];
        ret[0] = state;
        ret[0].servos.U = nextU;
        *len = 1;
    } else if (face == FACE_R) {
        uint8_t nextR = RotAfterMove[move.turns-1][servos.R];
        ret[0] = state;
        ret[0].servos.R = nextR;
        *len = 1;
    } else if (face == FACE_L) {
        uint8_t nextL = RotAfterMove[move.turns-1][servos.L];
        ret[0] = state;
        ret[0].servos.L = nextL;
        *len = 1;
    } else if (face == FACE_D) {
        uint8_t nextD = RotAfterMove[move.turns-1][servos.D];
        if (servos.n) {
            ret[0] = state; ret[0].servos.D = nextD;
            ret[1] = ret[0]; ret[1].servos.n = 0;
            *len = 2;
        } else {
            ret[0] = state; ret[0].servos.D = nextD;
            ret[1] = ret[0]; ret[2] = ret[0]; ret[3] = ret[0];
            ret[0].servos.n = 1;
            ret[1].servos.U = 0; ret[2].servos.U = 1; ret[3].servos.U = 2;
            *len = 4;
        }
    }
}
/*
def state_can_do_opposite_move_pair(move1: Move, move2: Move, state: State):
    face1 = Orientation_to_arr6[state.persp][faces_to_nums[move1.face]]
    if (face1 == 'F' or face1 == 'B'): return False
    if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
    if face1 == 'U': return (state.servos.R.rot != 1 and state.servos.L.rot != 1 and state.servos.U.rot != 3-move1.turns and state.servos.D.rot != 3-move2.turns)
    if face1 == 'R': return (state.servos.U.rot != 1 and state.servos.D.rot != 1 and state.servos.R.rot != 3-move1.turns and state.servos.L.rot != 3-move2.turns)
    if face1 == 'L': return (state.servos.U.rot != 1 and state.servos.D.rot != 1 and state.servos.L.rot != 3-move1.turns and state.servos.R.rot != 3-move2.turns)
    if face1 == 'D': return (state.servos.R.rot != 1 and state.servos.L.rot != 1 and state.servos.D.rot != 3-move1.turns and state.servos.U.rot != 3-move2.turns)
*/
bool state_can_do_opposite_move_pair(move_s move1, move_s move2, State_s state) {
    face_e face1 = Arr6_from_Orientation(state.persp).faces[move1.face];
    RobotState_s servos = state.servos;
    return (
        face1 != FACE_F && face1 != FACE_B && allequal5(servos.n, servos.e, servos.s, servos.w, 1) &&
        ((face1 == FACE_U && servos.R != 1 && servos.L != 1 && servos.U != 3-move1.turns && servos.D != 3-move2.turns) ||
        (face1 == FACE_R && servos.U != 1 && servos.D != 1 && servos.R != 3-move1.turns && servos.L != 3-move2.turns) ||
        (face1 == FACE_D && servos.R != 1 && servos.L != 1 && servos.D != 3-move1.turns && servos.U != 3-move2.turns) ||
        (face1 == FACE_L && servos.U != 1 && servos.D != 1 && servos.L != 3-move1.turns && servos.R != 3-move2.turns))
    );
}
/*
def state_after_opposite_moves_pair(move1: Move, move2: Move, state: State):
    if move1.face == 'D': return state_after_move(move2, state_after_move(move1, state)[0])
    if move2.face == 'D': return state_after_move(move1, state_after_move(move2, state)[0])
    return state_after_move(move2, state_after_move(move1, state)[0])
*/
void state_after_opposite_moves_pair(move_s move1, move_s move2, State_s state, uint8_t *len, State_s* ret) {
    if (move1.face == FACE_D) {
        state_after_move(move1, state, len, ret);
        state_after_move(move2, ret[0], len, ret);
        *len = 1;
    } else if (move2.face == FACE_D) {
        state_after_move(move2, state, len, ret);
        state_after_move(move1, ret[0], len, ret);
        *len = 1;
    } else {
        state_after_move(move1, state, len, ret);
        state_after_move(move2, ret[0], len, ret);
        *len = 1;
    }
}
static inline face_e thing(Orientation_s O, face_e face) {
    Orientation_arr6_s arr6 = Arr6_from_Orientation(O);
    for (face_e i = 0; i < 6; i++) {
        if (arr6.faces[i] == face) return i;
    }
}
bool endstate_can_do_MovePair_RSS(MovePair pair, uint32_t singleMoveQualifications) {
    if (MovePair_is_singleMove(pair)) {
        return (singleMoveQualifications>>(pair.move1.face*4 + (pair.move1.turns-1)))&1;
    } else {
        return (((singleMoveQualifications>>(pair.move1.face*4 + (pair.move1.turns-1)))&1) && 
               ((singleMoveQualifications>>(pair.move2.face*4 + (pair.move2.turns-1)))&1));
    }
}
bool endstate_can_do_move_nonRSS(move_s move, Orientation_s current_node_persp, Orientation_s endState_persp, uint32_t singleMoveQualifications) {
    uint8_t true_endState_perspNum = multiplied_orientations[current_node_persp.face*4 + current_node_persp.rot][endState_persp.face*4 + endState_persp.rot];
    face_e face = true_endState_perspNum>>2;
    uint8_t rot = true_endState_perspNum&3;
    Orientation_s O = {
        .face = face,
        .rot = rot
    };
    Orientation_arr6_s true_endState_arr6  = Arr6_from_Orientation(O);
    face_e facepos = true_endState_arr6.faces[move.face];
    face_e endState_moveface = thing(endState_persp, facepos);
    move_s movestruct = {.face = endState_moveface, .turns = move.turns};
    move_e moveenum = move_as_enum(movestruct);
    return ((singleMoveQualifications >> moveenum)&1);
}
bool state_can_do_MovePair(MovePair pair, State_s state) {
    if (MovePair_is_singleMove(pair)) {
        return state_can_do_move(pair.move1, state);
    } else {
        return state_can_do_opposite_move_pair(pair.move1, pair.move2, state);
    }
}
bool endstate_can_do_MovePair_nonRSS(MovePair pair, State_s current_node, State_s endState, uint32_t singleMoveQualifications) {
    /*
    Orientation_s current_node_persp = current_node.persp;
    Orientation_s endState_persp = endState.persp;
    bool ret;
    if (MovePair_is_singleMove(pair)) {
        ret = endstate_can_do_move_nonRSS(pair.move1, current_node_persp, endState_persp, singleMoveQualifications);
    } else {
        ret =  (endstate_can_do_move_nonRSS(pair.move1, current_node_persp, endState_persp, singleMoveQualifications) &&
                endstate_can_do_move_nonRSS(pair.move2, current_node_persp, endState_persp, singleMoveQualifications));
    }*/
    bool other = state_can_do_MovePair(pair, Undefault_EndState(current_node, endState));
    //if (other != ret) {
    //    print_State(current_node);
    //    printf("\n"); print_State(endState);
    //}
    return other;
}
void state_after_MovePair(MovePair pair, State_s state, uint8_t* len, State_s* ret) {
    if (MovePair_is_singleMove(pair)) {
        state_after_move(pair.move1, state, len, ret);
    } else {
        state_after_opposite_moves_pair(pair.move1, pair.move2, state, len, ret);
    }
}
void push_move_edges(MinHeap* minheap, MovePair pair, const MinHeapNode* current_node, size_t childN, uint8_t* stateAfterMove_len, State_s* stateAfterMove_arr) {
    state_after_MovePair(pair, current_node->state, stateAfterMove_len, stateAfterMove_arr);
    for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < *stateAfterMove_len; stateAfterMoveInd++) {
        MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], childN, false, current_node->weight + calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
    }
}

RobotSolution servoCode_compiler_Ofastest(const alg_s* alg, const inter_move_table_s* INTER_MOVE_TABLE) {
    /////////////////////////////////  LOAD ALG_CHUNKS  //////////////////////////////
    print_alg(alg);
    MovePair alg_sections[alg->length];
    uint8_t numAlgSecs = 0;
    for (int i = 0; i < alg->length; i++) {
        //if (numAlgSecs >= alg->length) printf("servoCode_compiler_Ofastest() done goofed up yo, numAlgSecs=%zu, alg->length=%zu\n", numAlgSecs, alg->length);
        move_s movestruct = move_as_struct[alg->moves[i]];
        //if (movestruct.face >= 6) printf("servoCode_compiler_Ofastest() done goofed up yo, movestruct.face=%zu\n", movestruct.face);
        if (numAlgSecs > 0 && alg_sections[numAlgSecs-1].move1.face == opposite_faces[movestruct.face]) {
            alg_sections[numAlgSecs-1].move2 = movestruct;
        } else {
            alg_sections[numAlgSecs] = (MovePair) {
                .move1 = movestruct,
                .move2 = NULL_MOVE
            }; numAlgSecs++;
        }
    }
    
    //////////////////////////// SUMMON EDSGAR W. DIJKSTRA ////////////////////////////
    //printf("Entering MinHeap_create()...\n");
    MinHeap* minheap = MinHeap_create(alg_sections, numAlgSecs);
    //printf("Finished MinHeap_create(), Updating MinHeap with ROBOT_START_STATE...\n");
    MinHeap_update_key(minheap, &ROBOT_START_STATE, -1, 0, 0, NULL);
    //printf("Finished updating MinHeap with ROBOT_START_STATE\n");
    State_s Robot_end_state;
    State_s* stateAfterMove_arr = malloc(4*sizeof(State_s));
    uint8_t stateAfterMove_len;
    Orientation_s DefaultOrientation = {
        .face = FACE_F,
        .rot = 0
    };

    MinHeapNode* current_node = MinHeap_pluck_min(minheap);

    const RSS_entry_s* RSS = inter_move_table_get_RSS(INTER_MOVE_TABLE);
    for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
        const RSS_sub_entry_s* path = &RSS->paths[pathInd];
        State_s endState = path->endState;
        if (endstate_can_do_MovePair_RSS(alg_sections[0], path->singleMoveQualifications)) {
            MinHeap_update_key(minheap, &endState, 0, true, path->weight, current_node);
        }
    }
    if (state_can_do_MovePair(alg_sections[0], current_node->state)) {
        push_move_edges(minheap, alg_sections[0], current_node, 0, &stateAfterMove_len, stateAfterMove_arr);
    }
    
    int record = 0;
    current_node = MinHeap_pluck_min(minheap);
    while (current_node->algorithm_index != numAlgSecs-1 || current_node->isBefore) {
        //if (current_node->algorithm_index == record && !current_node->isBefore) {
        //    printf("WE GOT TO END OF MOVE CHUNK %d\n", record++);
        //}
        uint8_t N = current_node->algorithm_index;
        if (current_node->isBefore) { // state is immediately before the move
            push_move_edges(minheap, alg_sections[N], current_node, N, &stateAfterMove_len, stateAfterMove_arr);
        } else if (!current_node->isBefore) { // state is immediately after the move
            const inter_move_entry_s* entry = inter_move_table_lookup(INTER_MOVE_TABLE, &current_node->state.servos);
            for (size_t pathInd = 0; pathInd < entry->length; pathInd++) {
                const sub_entry_s* path = &entry->paths[pathInd];
                State_s endState = Undefault_EndState(current_node->state, path->endState);
                if (endstate_can_do_MovePair_nonRSS(alg_sections[N+1], current_node->state, path->endState, path->singleMoveQualifications)) {
                    MinHeap_update_key(minheap, &endState, N+1, true, current_node->weight + path->weight, current_node);
                }
            }
            if (state_can_do_MovePair(alg_sections[N+1], current_node->state)) {
                push_move_edges(minheap, alg_sections[N+1], current_node, N+1, &stateAfterMove_len, stateAfterMove_arr);
            }
        }
        current_node = MinHeap_pluck_min(minheap);
    } free(stateAfterMove_arr);
    printf("DIJKSTRA FINISHED!: Min Distance: %lf\n", current_node->weight);

    ///////////// Trace and Save Dijkstra Solve //////////////
    size_t solve_path_length = 1;
    MinHeapNode* node = current_node;
    while(node->parent != NULL) {
        node = node->parent;
        solve_path_length++;
    }
    MinHeapNode DijkstraPath[solve_path_length];
    int index = solve_path_length-1;
    node = current_node;
    DijkstraPath[index--] = *node;
    while(node->parent != NULL) {
        node = node->parent;
        DijkstraPath[index--] = *node;
    }
    //printf("Finished saving DijkstraPath.\n");
    /////////////////////// Free the heap ////////////////////
    MinHeap_free(minheap);
    //////////////////// Fill in Gaps ///////////////////////
    RobotState_s* interpaths_paths[solve_path_length];
    for (size_t i = 0; i < solve_path_length; i++) interpaths_paths[i] = NULL;
    size_t interpaths_lengths[solve_path_length];
    for (size_t i = 0; i < solve_path_length; i++) interpaths_lengths[i] = 0;


    for (size_t i = 0; i < solve_path_length-1; i++) {
        if (State_is_ROBOT_START_STATE(&DijkstraPath[i].state) && DijkstraPath[i].algorithm_index == -1) {
            if (DijkstraPath[i+1].isBefore == 1) {
                const RSS_entry_s* RSS = inter_move_table_get_RSS(INTER_MOVE_TABLE);
                for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
                    if (compare_states(&DijkstraPath[i+1].state, &RSS->paths[pathInd].endState)) {
                        interpaths_lengths[i] = RSS->paths[pathInd].size;
                        interpaths_paths[i] = (RobotState_s*)malloc((interpaths_lengths[i])*sizeof(RobotState_s));
                        for (int j = 0; j < interpaths_lengths[i]; j++) {
                            interpaths_paths[i][j] = RSS->paths[pathInd].path[j].servos;
                        }
                        break;
                    }
                }
            }
        } else if (DijkstraPath[i].isBefore == 1) {
            const inter_move_entry_s* entry = inter_move_table_lookup(INTER_MOVE_TABLE, &DijkstraPath[i].state.servos);
            Orientation_arr6_s Arr6 = Arr6_from_Orientation(DijkstraPath[i].state.persp);
            for (size_t pathInd = 0; pathInd < entry->length; pathInd++) {
                Orientation_arr6_s nextArr6 = Arr6_from_Orientation(entry->paths[pathInd].endState.persp);
                State_s endState = Undefault_EndState(DijkstraPath[i].state, entry->paths[pathInd].endState);
                if (compare_states(&DijkstraPath[i+1].state, &endState)) {
                    interpaths_lengths[i] = entry->paths[pathInd].size;
                    interpaths_paths[i] = (RobotState_s*)malloc((interpaths_lengths[i])*sizeof(RobotState_s));
                    for (int j = 0; j < interpaths_lengths[i]; j++) {
                        interpaths_paths[i][j] = entry->paths[pathInd].path[j];
                    }
                    break;
                }
            }
        }
    }

    size_t ROBOT_SOLUTION_LENGTH = solve_path_length;
    for (size_t i = 0; i < solve_path_length; i++) {
        ROBOT_SOLUTION_LENGTH += interpaths_lengths[i];
    }


    RobotState_s* ROBOT_SOLUTION = (RobotState_s*)malloc(ROBOT_SOLUTION_LENGTH * sizeof(RobotState_s));
    index = 0;
    for (size_t i = 0; i < solve_path_length; i++) {
        ROBOT_SOLUTION[index++] = DijkstraPath[i].state.servos;
        for (size_t j = 0; j < interpaths_lengths[i]; j++) {
            ROBOT_SOLUTION[index++] = interpaths_paths[i][j];
        } free(interpaths_paths[i]);
    }
    return (RobotSolution) {
        .solution = ROBOT_SOLUTION,
        .size = ROBOT_SOLUTION_LENGTH
    };
}