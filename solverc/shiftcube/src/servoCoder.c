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
#include <assert.h>

#define INTER_MOVE_TABLE_CAPACITY 162
#define INTER_MOVE_TABLE_PATHS_PER_NODE_NONRSS 735
#define INTER_MOVE_TABLE_PATHS_PER_NODE_RSS 736

/*
#define halfTurn 500 //in Milliseconds
#define quarterTurn 350 //in Milliseconds
#define engageTime 375 //in Milliseconds
*/
#define Etime 0.300
#define dEtime 0.300
#define rot1time 0.35
#define rot2time 0.5

typedef struct {
    face_e faces[6];
} Orientation_arr6_s; // 6 Bytes
typedef struct DijkstraPath {
    MinHeapNode* path;
    size_t size;
} DijkstraPath_s;

typedef struct {                                                  // There will be 735 of these lying around
    State_s endState; // 4 bytes
    uint32_t singleMoveQualifications; // 4 bytes
    float distance; // 4 bytes
    float action;
    State_s* path; // 4 on 32-bit and 8 on 64-bit
    size_t size; // 4 on 32-bit and 8 on 64-bit                          // In total, these sizes will amount to 6672  ****CHANGE THIS AFTER PROTOCOL_SEARCH
} RSS_sub_entry_s; // 16 on 32-bit and 24 on 64-bit
typedef struct {                                                  // There will be 1 of these lying around
    State_s startState; // 4 bytes
    RSS_sub_entry_s* paths; // // 4 on 32-bit and 8 on 64-bit
    size_t length; // 4 on 32-bit and 8 on 64-bit
    size_t size;// 4 on 32-bit and 8 on 64-bit
} RSS_entry_s; // 16 on 32-bit and 28 on 64-bit

typedef struct {                                                  // There will be 67620 of these lying around
    State_s endState; // 4 bytes
    uint32_t singleMoveQualifications; // 4 bytes
    float distance; // 4 bytes
    float action;
    RobotState_s* path; // 4 on 32-bit and 8 on 64-bit
    size_t size; // 4 on 32-bit and 8 on 64-bit                          // In total, these sizes will amount to 587364
} sub_entry_s; // 16 on 32-bit and 24 on 64-bit
typedef struct {                                                  // There will be 162 of these lying around
    RobotState_s startState; // 2 bytes
    sub_entry_s* paths; // 4 on 32-bit and 8 on 64-bit
    size_t length; // 4 on 32-bit and 8 on 64-bit
    size_t size; // 4 on 32-bit and 8 on 64-bit
} inter_move_entry_s; // 14 on 32-bit and 26 on 64-bit

typedef struct inter_move_table {
    RSS_entry_s RSS; // already counted for
    size_t size; // 4 on 32-bit and 8 on 64-bit
    inter_move_entry_s *table; // 4 on 32-bit and 8 on 64-bit
} inter_move_table_s; // 8 new bytes on 32-bit and 16 new bytes on 64-bit
//    So on total, this inter-move-table will take up:                  *                                             *
//                                                on 32-bit, (11760 + 26688 + 16 + 1081920 + 1174728 + 2268 + 8)  = 2297388 bytes
//                                                on 64-bit, (17640 + 26688 + 28 + 1622880 + 1174728 + 4212 + 16) = 2846192 bytes

//////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////
//               R        L        U         D
// (('R', 2), (90, 1), (90, 1), (180, 0), (0, 0))    
static const State_s ROBOT_START_STATE = {
    .persp = (Orientation_s) {
        FACE_R, 0
    },
    .servos = (RobotState_s) {
        0,1,0,1,0,1,2,1
    }
};

static const State_s NULL_STATE = {
    .persp = {
        .face = FACE_NULL,
        .rot = 0
    },
    .servos = {
        0,0,0,0,0,0,0,0
    }
};
//////////////////////////////////////////////////// TABLES ////////////////////////////////////////////
static const move_e move_through_orientationNum[24][18] = {
	{MOVE_F, MOVE_F2, MOVE_F3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3},
	{MOVE_F, MOVE_F2, MOVE_F3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3},
	{MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_B, MOVE_B2, MOVE_B3},
	{MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_B, MOVE_B2, MOVE_B3},
	{MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3},
	{MOVE_L, MOVE_L2, MOVE_L3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3},
	{MOVE_D, MOVE_D2, MOVE_D3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_U, MOVE_U2, MOVE_U3},
	{MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_L, MOVE_L2, MOVE_L3},
	{MOVE_U, MOVE_U2, MOVE_U3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3},
	{MOVE_L, MOVE_L2, MOVE_L3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3},
	{MOVE_D, MOVE_D2, MOVE_D3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_U, MOVE_U2, MOVE_U3},
	{MOVE_R, MOVE_R2, MOVE_R3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_L, MOVE_L2, MOVE_L3},
	{MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3},
	{MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_R, MOVE_R2, MOVE_R3},
	{MOVE_D, MOVE_D2, MOVE_D3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_U, MOVE_U2, MOVE_U3},
	{MOVE_R, MOVE_R2, MOVE_R3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3},
	{MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_D, MOVE_D2, MOVE_D3},
	{MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_R, MOVE_R2, MOVE_R3},
	{MOVE_D, MOVE_D2, MOVE_D3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_U, MOVE_U2, MOVE_U3},
	{MOVE_R, MOVE_R2, MOVE_R3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_F, MOVE_F2, MOVE_F3, MOVE_L, MOVE_L2, MOVE_L3},
	{MOVE_B, MOVE_B2, MOVE_B3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_F, MOVE_F2, MOVE_F3},
	{MOVE_B, MOVE_B2, MOVE_B3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_F, MOVE_F2, MOVE_F3},
	{MOVE_B, MOVE_B2, MOVE_B3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_F, MOVE_F2, MOVE_F3},
	{MOVE_B, MOVE_B2, MOVE_B3, MOVE_D, MOVE_D2, MOVE_D3, MOVE_R, MOVE_R2, MOVE_R3, MOVE_U, MOVE_U2, MOVE_U3, MOVE_L, MOVE_L2, MOVE_L3, MOVE_F, MOVE_F2, MOVE_F3},
};
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
static bool RobotStateNum_can_do_move[18][1296];

/////////////////////////////////////////////// PRIVATE FUNCTION PROTOTYPES /////////////////////////////////////////
bool inter_move_table_insert(inter_move_table_s *ht, const char* line);
bool inter_move_table_RSS_insert(inter_move_table_s *ht, const char* line);
void insert_normal_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, const char *filename);
void insert_root_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, const char *filename);
static inline size_t inter_move_table_get_index(const inter_move_table_s *ht, const RobotState_s *key);
const RSS_entry_s* inter_move_table_get_RSS(const inter_move_table_s *ht);
const inter_move_entry_s* inter_move_table_lookup(const inter_move_table_s *ht, const RobotState_s *key);

Orientation_arr6_s Arr6_from_Orientation(Orientation_s O);
Orientation_s Orientation_from_Arr6(Orientation_arr6_s arr6);
Orientation_arr6_s multiply_arr6s(Orientation_arr6_s arr1, Orientation_arr6_s arr2);
static inline uint8_t orientationNum(Orientation_s O);
static inline Orientation_s orientation_from_num(uint8_t num);
State_s Undefault_EndState(State_s origin, State_s OGendState);
State_s stateNum_to_state(uint16_t stateNum);
void print_RobotState(RobotState_s servos);
uint16_t RobotState_to_2B(const RobotState_s* state);
RobotState_s RobotStateNum_to_RobotState(uint16_t RobotStateNum);
uint16_t RobotState_to_RobotStateNum(const RobotState_s* state);
bool compare_RobotStates(const RobotState_s* state1, const RobotState_s* state2);
bool State_is_ROBOT_START_STATE(const State_s* state);

bool is_valid_state(const State_s* state);
float calc_weight_of_armstep(bool e1, uint8_t rot1, bool e2, uint8_t rot2);
float calc_weight_of_step(const State_s* state1, const State_s* state2);
float calc_action_of_step(const State_s* state1, const State_s* state2);

static inline bool allequal5(bool a1, bool a2, bool a3, bool a4, bool a5);
static inline bool allequal4(bool a1, bool a2, bool a3, bool a4);

bool can_defaultState_do_move(move_e move, RobotState_s servos);
void init_RobotStateNum_can_do_move();
bool state_can_do_move(move_e move, State_s state);
bool endstate_can_do_MovePair_RSS(MovePair pair, uint32_t singleMoveQualifications);
bool state_can_do_MovePair(MovePair pair, State_s state);
void state_after_move(move_e move, State_s state, uint8_t *len, State_s* ret);
bool state_can_do_opposite_move_pair(move_e move1, move_e move2, State_s state);
void state_after_opposite_moves_pair(move_e move1, move_e move2, State_s state, uint8_t *len, State_s* ret);
void state_after_MovePair(MovePair pair, State_s state, uint8_t* len, State_s* ret);

void push_move_edges(MinHeap* minheap, MovePair pair, MinHeapNode* current_node, size_t childN, uint8_t* stateAfterMove_len, State_s* stateAfterMove_arr);
void Load_alg_chunks(const alg_s* alg, MovePair* alg_sections, uint8_t* numAlgSecs);
void servoCode_compiler_Dijkstra(MinHeap* minheap, MovePair* alg_sections, uint8_t numAlgSecs, const inter_move_table_s* INTER_MOVE_TABLE, MinHeapNode** EndNode);
DijkstraPath_s Form_DijkstraPath_from_EndNode(MinHeapNode* EndNode);
RobotSolution Form_RobotSolution_from_DijkstraPath(DijkstraPath_s Dijkstra, const inter_move_table_s* INTER_MOVE_TABLE);

/////////////////////////////////////////////// FUNCTIONS /////////////////////////////////////////

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
        arr_of_stateNumStrs[i] = (char*)malloc(sizeof(char)*(stateNumStrLengths[i]+1));
        arr_of_stateNumStrs[i][stateNumStrLengths[i]] = 0;
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
    float total_action = 0;
    for (int i = 0; i < space_count; ++i) {
        total_weight += calc_weight_of_step(&arr_of_states[i], &arr_of_states[i+1]);
        total_action += calc_action_of_step(&arr_of_states[i], &arr_of_states[i+1]);
    }

    ///////////////////////////////////// INSERT NEW PATH ONTO NODE //////////////////////////////

    index = inter_move_table_hash(&(arr_of_states[0].servos));

    if (ht->table[index].paths == NULL) {
        ht->table[index].startState = arr_of_states[0].servos;
        ht->table[index].paths = (sub_entry_s*)malloc(INTER_MOVE_TABLE_PATHS_PER_NODE_NONRSS*sizeof(sub_entry_s));
        ht->table[index].length = 0;
        ht->table[index].size = INTER_MOVE_TABLE_PATHS_PER_NODE_NONRSS;
    }

    sub_entry_s sub_entry = {
        .endState = arr_of_states[space_count],
        .singleMoveQualifications = 0,
        .distance = total_weight,
        .action = total_action,
        .path = (RobotState_s*)malloc(sizeof(RobotState_s)*(space_count-1)),
        .size = space_count-1
    };
    for (int i = 1; i < space_count; ++i) {
        sub_entry.path[i-1] = arr_of_states[i].servos;
    }
    for (move_e move = 0; move < 18; move++) {
        if (state_can_do_move(move, sub_entry.endState)) {
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
        arr_of_stateNumStrs[i] = (char*)malloc(sizeof(char)*(stateNumStrLengths[i]+1));
        arr_of_stateNumStrs[i][stateNumStrLengths[i]] = 0;
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
    float total_action = 0;
    for (int i = 0; i < space_count; ++i) {
        total_weight += calc_weight_of_step(&arr_of_states[i], &arr_of_states[i+1]);
        total_action += calc_action_of_step(&arr_of_states[i], &arr_of_states[i+1]);
    }

    ///////////////////////////////////// INSERT NEW PATH ONTO NODE //////////////////////////////
    ht->RSS.paths[ht->RSS.length] = (RSS_sub_entry_s) {
        .endState = arr_of_states[space_count],
        .singleMoveQualifications = 0,
        .distance = total_weight,
        .action = total_action,
        .path = (State_s*)malloc(sizeof(State_s)*(space_count-1)),
        .size = space_count-1
    };
    for (move_e move = 0; move < 18; move++) {
        if (state_can_do_move(move, ht->RSS.paths[ht->RSS.length].endState)) {
            ht->RSS.paths[ht->RSS.length].singleMoveQualifications |= (1<<move);
        }
    }
    for (int i = 1; i < space_count; ++i) {
        ht->RSS.paths[ht->RSS.length].path[i-1] = arr_of_states[i];
    } ht->RSS.length++;

    return true;
}
void insert_normal_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, const char *filename) {
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
void insert_root_lines_into_inter_move_table(inter_move_table_s* INTER_MOVE_TABLE, const char *filename) {
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
    ht->RSS.paths = (RSS_sub_entry_s*)calloc(INTER_MOVE_TABLE_PATHS_PER_NODE_RSS, sizeof(RSS_sub_entry_s));

    ht->RSS.length = 0;
    ht->RSS.size = INTER_MOVE_TABLE_PATHS_PER_NODE_RSS;
    ht->RSS.startState = ROBOT_START_STATE;
    ht->size = INTER_MOVE_TABLE_CAPACITY;

    init_RobotStateNum_can_do_move();

    insert_normal_lines_into_inter_move_table(ht, INTER_MOVE_TABLE_PATH);
    insert_root_lines_into_inter_move_table(ht, INTER_MOVE_TABLE_RSS_PATH);

    /*
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
    }*/
    /*
    printf("In the INTER_MOVE_TABLE, there are %zu SubEntries, %zu RSSSubEntries, %zu InterPathNodes, %zu RSSInterPathNodes\n", 
        numSubEntries,
        numRSSSubEntries,
        numInterPathNodes,
        numRSSInterPathNodes
    ); */

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
static inline uint8_t orientationNum(Orientation_s O) {
    return O.face*4 + O.rot;
}
static inline Orientation_s orientation_from_num(uint8_t num) {
    return (Orientation_s) {
        .face = num>>2,
        .rot = num&3
    };

}
State_s Undefault_EndState(State_s origin, State_s OGendState) {
    State_s true_endState1 = OGendState;
    true_endState1.persp = orientation_from_num(multiplied_orientations[orientationNum(origin.persp)][orientationNum(OGendState.persp)]);
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
void print_RobotState(RobotState_s servos) {
    printf("%c.%c.%c.%c.U%hu.R%hu.D%hu.L%hu",
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
uint16_t RobotState_to_2B(const RobotState_s* state) {
    uint16_t bitmask = 0;
    bitmask <<= 1; bitmask |= state->n;
    bitmask <<= 1; bitmask |= state->e;
    bitmask <<= 1; bitmask |= state->s;
    bitmask <<= 1; bitmask |= state->w;
    bitmask <<= 2; bitmask |= state->U;
    bitmask <<= 2; bitmask |= state->R;
    bitmask <<= 2; bitmask |= state->D;
    bitmask <<= 2; bitmask |= state->L;
    return bitmask;
}
RobotState_s RobotStateNum_to_RobotState(uint16_t RobotStateNum) {
    RobotState_s RobotState;
    uint16_t copy = RobotStateNum;
    RobotState.L = copy%3; copy /= 3;
    RobotState.D = copy%3; copy /= 3;
    RobotState.R = copy%3; copy /= 3;
    RobotState.U = copy%3; copy /= 3;

    RobotState.w = copy%2; copy /= 2;
    RobotState.s = copy%2; copy /= 2;
    RobotState.e = copy%2; copy /= 2;
    RobotState.n = copy%2; copy /= 2;
    return RobotState;
}
uint16_t RobotState_to_RobotStateNum(const RobotState_s* state) {
    uint16_t bitmask = 0;
    bitmask <<= 1; bitmask |= state->n;
    bitmask <<= 1; bitmask |= state->e;
    bitmask <<= 1; bitmask |= state->s;
    bitmask <<= 1; bitmask |= state->w;
    bitmask *= 3; bitmask += state->U;
    bitmask *= 3; bitmask += state->R;
    bitmask *= 3; bitmask += state->D;
    bitmask *= 3; bitmask += state->L;
    return bitmask;
}
uint16_t RobotState_to_uint16t(const RobotState_s* state) {
    uint16_t bitmask = 0;
    bitmask <<= 1; bitmask |= state->n;
    bitmask <<= 1; bitmask |= state->e;
    bitmask <<= 1; bitmask |= state->s;
    bitmask <<= 1; bitmask |= state->w;
    bitmask <<= 2; bitmask |= state->U;
    bitmask <<= 2; bitmask |= state->R;
    bitmask <<= 2; bitmask |= state->D;
    bitmask <<= 2; bitmask |= state->L;
    return bitmask;
}

bool compare_RobotStates(const RobotState_s* state1, const RobotState_s* state2) {
    return (
        RobotState_to_2B(state1) == RobotState_to_2B(state2)
    );
}

bool compare_states(const State_s* state1, const State_s* state2) {
    return (
        compare_RobotStates(&(state1->servos), &(state2->servos)) &&
        state1->persp.face == state2->persp.face &&
        state1->persp.rot == state2->persp.rot
    );
}
bool State_is_ROBOT_START_STATE(const State_s* state) {
    return compare_states(state, &ROBOT_START_STATE);
}
bool is_valid_state(const State_s* state) {
    if (state->servos.R == 1 && 1 == state->servos.D && state->servos.e == 1 && 1 == state->servos.s) return false;
    if (state->servos.L == 1 && 1 == state->servos.D && state->servos.w == 1 && 1 == state->servos.s) return false;
    if (state->servos.R == 1 && 1 == state->servos.U && state->servos.e == 1 && 1 == state->servos.n) return false;
    if (state->servos.L == 1 && 1 == state->servos.U && state->servos.w == 1 && 1 == state->servos.n) return false;
    return (state->servos.s == 1 || 
           ((state->servos.R == 1 && state->servos.e == 1) && state->servos.w == 1) || 
           ((state->servos.L == 1 && state->servos.w == 1) && state->servos.e == 1));
}

float calc_weight_of_armstep(bool e1, uint8_t rot1, bool e2, uint8_t rot2) {
    float ret;
    if (e1 == 0 && e2 == 1) ret = (float)Etime;
    else if (e1 == 1 && e2 == 0) ret = (float)dEtime;
    else if (abs(rot2 - rot1) == 1) ret = (float)rot1time;
    else if (abs(rot2 - rot1) == 2) ret = (float)rot2time;
    else ret = 0;
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
float calc_action_of_step(const State_s* state1, const State_s* state2) {
    float maxWeight = 0;
    float arm1 = calc_weight_of_armstep(state1->servos.n, state1->servos.U, state2->servos.n, state2->servos.U);
    float arm2 = calc_weight_of_armstep(state1->servos.e, state1->servos.R, state2->servos.e, state2->servos.R);
    float arm3 = calc_weight_of_armstep(state1->servos.s, state1->servos.D, state2->servos.s, state2->servos.D);
    float arm4 = calc_weight_of_armstep(state1->servos.w, state1->servos.L, state2->servos.w, state2->servos.L);
    return arm1 + arm2 + arm3 + arm4;
}

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
bool can_defaultState_do_move(move_e move, RobotState_s servos) {
    face_e face = move_faces[move];
    if (face == FACE_F || face == FACE_B || face == FACE_NULL) return false;
    bool ret = false;
    if (face == FACE_U) ret = ((allequal5(servos.n, servos.e, servos.s, servos.w, 1)) && (servos.R != 1 && servos.L != 1) && (servos.U != 3-move_turns[move]));
    if (face == FACE_R) ret = ((allequal5(servos.n, servos.e, servos.s, servos.w, 1)) && (servos.U != 1 && servos.D != 1) && (servos.R != 3-move_turns[move]));
    if (face == FACE_D) ret = ((allequal4(servos.e, servos.s, servos.w, 1)) && (servos.R != 1 && servos.L != 1) && (servos.D != 3-move_turns[move]));
    if (face == FACE_L) ret = ((allequal5(servos.n, servos.e, servos.s, servos.w, 1)) && (servos.U != 1 && servos.D != 1) && (servos.L != 3-move_turns[move]));
    return ret;
}
void init_RobotStateNum_can_do_move() {
    for (int RobotStateNum = 0; RobotStateNum < 1296; RobotStateNum++) {
        State_s state = {.persp = {.face = FACE_F, .rot = 0}, .servos = RobotStateNum_to_RobotState(RobotStateNum)};
        if (!is_valid_state(&state)) continue;
        for (move_e move = 0; move < 18; move++) {
            //printf("we reached this point!\n");
            RobotStateNum_can_do_move[move][RobotStateNum] = can_defaultState_do_move(move, state.servos);
        }
    }/*
    printf("static const bool RobotStateNum_can_do_move[18][1296] = {\n");
    for (int i = 0; i < 18; i++) {
        printf("\t{");
        for (int j = 0; j < 1296; j++) {
            printf("%hhu,", RobotStateNum_can_do_move[i][j]);
        } printf("},\n");
    } printf("};");*/
}
bool state_can_do_move(move_e move, State_s state) {
    return RobotStateNum_can_do_move[move_through_orientationNum[orientationNum(state.persp)][move]][RobotState_to_RobotStateNum(&state.servos)];
}

void state_after_move(move_e move, State_s state, uint8_t *len, State_s* ret) {
    face_e face = move_faces[move_through_orientationNum[orientationNum(state.persp)][move]];
    RobotState_s servos = state.servos;
    uint8_t RotAfterMove[3][3] = {
        {1, 2, 4},
        {2, 4, 0},
        {4, 0, 1}
    };
    if (face == FACE_U) {
        uint8_t nextU = RotAfterMove[move_turns[move]-1][servos.U];
        ret[0] = state;
        ret[0].servos.U = nextU;
        *len = 1;
    } else if (face == FACE_R) {
        uint8_t nextR = RotAfterMove[move_turns[move]-1][servos.R];
        ret[0] = state;
        ret[0].servos.R = nextR;
        *len = 1;
    } else if (face == FACE_L) {
        uint8_t nextL = RotAfterMove[move_turns[move]-1][servos.L];
        ret[0] = state;
        ret[0].servos.L = nextL;
        *len = 1;
    } else if (face == FACE_D) {
        uint8_t nextD = RotAfterMove[move_turns[move]-1][servos.D];
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

bool state_can_do_opposite_move_pair(move_e move1, move_e move2, State_s state) {
    uint16_t RobotStateNum = RobotState_to_RobotStateNum(&state.servos);
    uint8_t perspNum = orientationNum(state.persp);
    return (RobotStateNum_can_do_move[move_through_orientationNum[perspNum][move1]][RobotStateNum] &&
            RobotStateNum_can_do_move[move_through_orientationNum[perspNum][move2]][RobotStateNum]);
}

void state_after_opposite_moves_pair(move_e move1, move_e move2, State_s state, uint8_t *len, State_s* ret) {
    if (move_faces[move1] == FACE_D) {
        state_after_move(move1, state, len, ret);
        state_after_move(move2, ret[0], len, ret);
        *len = 1;
    } else if (move_faces[move2] == FACE_D) {
        state_after_move(move2, state, len, ret);
        state_after_move(move1, ret[0], len, ret);
        *len = 1;
    } else {
        state_after_move(move1, state, len, ret);
        state_after_move(move2, ret[0], len, ret);
        *len = 1;
    }
}
bool endstate_can_do_MovePair_RSS(MovePair pair, uint32_t singleMoveQualifications) {
    if (MovePair_is_singleMove(pair)) {
        return (singleMoveQualifications>>pair.move1)&1;
    } else {
        return (((singleMoveQualifications>>pair.move1)&1) && 
               ((singleMoveQualifications>>pair.move2)&1));
    }
}
bool state_can_do_MovePair(MovePair pair, State_s state) {
    if (MovePair_is_singleMove(pair)) {
        return state_can_do_move(pair.move1, state);
    } else {
        return state_can_do_opposite_move_pair(pair.move1, pair.move2, state);
    }
}
void state_after_MovePair(MovePair pair, State_s state, uint8_t* len, State_s* ret) {
    if (MovePair_is_singleMove(pair)) {
        state_after_move(pair.move1, state, len, ret);
    } else {
        state_after_opposite_moves_pair(pair.move1, pair.move2, state, len, ret);
    }
}
void push_move_edges(MinHeap* minheap, MovePair pair, MinHeapNode* current_node, size_t childN, uint8_t* stateAfterMove_len, State_s* stateAfterMove_arr) {
    state_after_MovePair(pair, current_node->state, stateAfterMove_len, stateAfterMove_arr);
    for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < *stateAfterMove_len; stateAfterMoveInd++) {
        MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], childN, false, current_node->distance + calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node->action + calc_action_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
    }
}

void Load_alg_chunks(const alg_s* alg, MovePair* alg_sections, uint8_t* numAlgSecs) {
    /////////////////////////////////  LOAD ALG_CHUNKS  //////////////////////////////
    //print_alg(alg);
    for (int i = 0; i < alg->length; i++) {
        if (*numAlgSecs > 0 && move_faces[alg_sections[*numAlgSecs-1].move1] == opposite_faces[move_faces[alg->moves[i]]]) {
            alg_sections[*numAlgSecs-1].move2 = alg->moves[i];
        } else {
            alg_sections[*numAlgSecs] = (MovePair) {
                .move1 = alg->moves[i],
                .move2 = MOVE_NULL
            }; (*numAlgSecs)++;
        }
    }
}

size_t total_nodes_from_alg_secs(MovePair* alg_sections, uint8_t numAlgSecs) {
    size_t numSingleMoves = 0;
    size_t numOppPairs = 0;
    for (size_t i = 0; i < numAlgSecs; i++) {
        if (MovePair_is_singleMove(alg_sections[i])) {
            numSingleMoves++;
        } else numOppPairs++;
    }
    return (480*2*numSingleMoves) + (256*2*numOppPairs) + 1;
}

void servoCode_compiler_Dijkstra(MinHeap* minheap, MovePair* alg_sections, uint8_t numAlgSecs, const inter_move_table_s* INTER_MOVE_TABLE, MinHeapNode** EndNode) {
    MinHeap_update_key(minheap, &ROBOT_START_STATE, -1, 0, 0, 0, NULL); //printf("\tline 793\n");
    State_s* stateAfterMove_arr = malloc(4*sizeof(State_s));
    uint8_t stateAfterMove_len;

    MinHeapNode* current_node = MinHeap_pluck_min(minheap); //printf("\tline 797\n");

    const RSS_entry_s* RSS = inter_move_table_get_RSS(INTER_MOVE_TABLE);
    if (MovePair_is_singleMove(alg_sections[0])) { //printf("\tline 822\n");
        for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
            const RSS_sub_entry_s* path = &RSS->paths[pathInd];
            State_s endState = path->endState; //printf("\t\t\tsingleMoveQualifications was %zu\n", path->singleMoveQualifications);
            if ((((path->singleMoveQualifications)>>(alg_sections[0].move1))&1)) {
                MinHeap_update_key(minheap, &endState, 0, true, path->distance, path->action, current_node);
            }
        }
    } else { //printf("\tline 830\n");
        for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
            const RSS_sub_entry_s* path = &RSS->paths[pathInd];
            State_s endState = path->endState;
            if ((((path->singleMoveQualifications)>>alg_sections[0].move1)&1) && (((path->singleMoveQualifications)>>alg_sections[0].move2)&1)) {
                MinHeap_update_key(minheap, &endState, 0, true, path->distance, path->action, current_node);
            }
        }
    }
    if (state_can_do_MovePair(alg_sections[0], current_node->state)) {
        push_move_edges(minheap, alg_sections[0], current_node, 0, &stateAfterMove_len, stateAfterMove_arr);
    } //printf("\tline 819\n");
    
    current_node = MinHeap_pluck_min(minheap); //printf("\tline 821\n");
    while (current_node->algorithm_index != numAlgSecs-1 || current_node->isBefore) {
        //if (current_node->algorithm_index == record && !current_node->isBefore) {
        //    printf("WE GOT TO END OF MOVE CHUNK %d\n", record++);
        //}
        uint8_t N = current_node->algorithm_index;
        if (current_node->isBefore) { // state is immediately before the move
            push_move_edges(minheap, alg_sections[N], current_node, N, &stateAfterMove_len, stateAfterMove_arr);
        } else if (!current_node->isBefore) { // state is immediately after the move
            MovePair pair = alg_sections[N+1];
            const inter_move_entry_s* entry = inter_move_table_lookup(INTER_MOVE_TABLE, &current_node->state.servos);

            if (MovePair_is_singleMove(pair)) {
                pair.move1 = move_through_orientationNum[orientationNum(current_node->state.persp)][pair.move1];
                for (size_t pathInd = 0; pathInd < entry->length; pathInd++) {
                    const sub_entry_s* path = &entry->paths[pathInd];
                    State_s endState = Undefault_EndState(current_node->state, path->endState);
                    if ((((path->singleMoveQualifications)>>pair.move1)&1)) {
                        MinHeap_update_key(minheap, &endState, N+1, true, current_node->distance + path->distance, current_node->action + path->action, current_node);
                    }
                }
            } else {
                pair.move1 = move_through_orientationNum[orientationNum(current_node->state.persp)][pair.move1];
                pair.move2 = move_through_orientationNum[orientationNum(current_node->state.persp)][pair.move2];
                for (size_t pathInd = 0; pathInd < entry->length; pathInd++) {
                    const sub_entry_s* path = &entry->paths[pathInd];
                    State_s endState = Undefault_EndState(current_node->state, path->endState);
                    if ((((path->singleMoveQualifications)>>pair.move1)&1) && (((path->singleMoveQualifications)>>pair.move2)&1)) {
                        MinHeap_update_key(minheap, &endState, N+1, true, current_node->distance + path->distance, current_node->action + path->action, current_node);
                    }
                }
            }
            if (state_can_do_MovePair(alg_sections[N+1], current_node->state)) {
                push_move_edges(minheap, alg_sections[N+1], current_node, N+1, &stateAfterMove_len, stateAfterMove_arr);
            }
        }
        current_node = MinHeap_pluck_min(minheap);
    } free(stateAfterMove_arr);
    MinHeapNode* new_node = NULL;
    do {
        if (new_node && new_node->action < current_node->action && 
            new_node->algorithm_index == numAlgSecs-1 && 
            !(new_node->isBefore)
        ) current_node = new_node;
        new_node = MinHeap_pluck_min(minheap);
    } while (new_node->distance == current_node->distance);
    *EndNode = current_node; //printf("\tline 860\n");
}
DijkstraPath_s Form_DijkstraPath_from_EndNode(MinHeapNode* EndNode) {
    size_t solve_path_length = 1;
    MinHeapNode* node = EndNode;
    while(node->parent != NULL) {
        node = node->parent;
        solve_path_length++;
    }
    DijkstraPath_s DijkstraPath;
    DijkstraPath.path = (MinHeapNode*)malloc(solve_path_length*sizeof(MinHeapNode));
    DijkstraPath.size = solve_path_length;
    int index = solve_path_length-1;
    node = EndNode;
    do {
        DijkstraPath.path[index--] = *node;
        node = node->parent;
    } while(node);
    return DijkstraPath;
}
RobotSolution Form_RobotSolution_from_DijkstraPath(DijkstraPath_s Dijkstra, const inter_move_table_s* INTER_MOVE_TABLE) {
    //for (int i = 0; i < Dijkstra.size; i++) {
    //    print_State(Dijkstra.path[i].state); printf(", isBefore=%hhu, weight=%f\n", Dijkstra.path[i].isBefore, Dijkstra.path[i].weight);
    //} printf("----------------------------\n");

    RobotState_s* interpaths_paths[Dijkstra.size];
    for (size_t i = 0; i < Dijkstra.size; i++) interpaths_paths[i] = NULL;
    size_t interpaths_lengths[Dijkstra.size];
    for (size_t i = 0; i < Dijkstra.size; i++) interpaths_lengths[i] = 0;

    if (Dijkstra.path[0+1].isBefore == 1) {
        const RSS_entry_s* RSS = inter_move_table_get_RSS(INTER_MOVE_TABLE);
        for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
            if (compare_states(&Dijkstra.path[0+1].state, &RSS->paths[pathInd].endState)) {
                interpaths_lengths[0] = RSS->paths[pathInd].size;
                interpaths_paths[0] = (RobotState_s*)malloc((interpaths_lengths[0])*sizeof(RobotState_s));
                for (int j = 0; j < interpaths_lengths[0]; j++) {
                    interpaths_paths[0][j] = RSS->paths[pathInd].path[j].servos;
                }
                break;
            }
        }
    }
    for (size_t i = 1; i < Dijkstra.size-1; i++) {
        if (Dijkstra.path[i+1].isBefore == 1) {
            const inter_move_entry_s* entry = inter_move_table_lookup(INTER_MOVE_TABLE, &Dijkstra.path[i].state.servos);
            for (size_t pathInd = 0; pathInd < entry->length; pathInd++) {
                State_s endState = Undefault_EndState(Dijkstra.path[i].state, entry->paths[pathInd].endState);
                if (compare_states(&Dijkstra.path[i+1].state, &endState)) {
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

    size_t ROBOT_SOLUTION_LENGTH = Dijkstra.size;
    for (size_t i = 0; i < Dijkstra.size; i++) {
        ROBOT_SOLUTION_LENGTH += interpaths_lengths[i];
    }


    RobotState_s* ROBOT_SOLUTION = (RobotState_s*)malloc(ROBOT_SOLUTION_LENGTH * sizeof(RobotState_s));
    size_t index = 0;
    for (size_t i = 0; i < Dijkstra.size; i++) {
        //print_State(Dijkstra.path[i].state); printf("\n");
        ROBOT_SOLUTION[index++] = Dijkstra.path[i].state.servos;
        for (size_t j = 0; j < interpaths_lengths[i]; j++) {
            //print_RobotState(interpaths_paths[i][j]); printf("\n");
            ROBOT_SOLUTION[index++] = interpaths_paths[i][j];
        } free(interpaths_paths[i]);
    } //printf("----------------------------\n");
    return (RobotSolution) {
        .solution = ROBOT_SOLUTION,
        .size = ROBOT_SOLUTION_LENGTH
    };
}

RobotSolution servoCode_compiler_Ofastest(const alg_s* alg, const inter_move_table_s* INTER_MOVE_TABLE) {
    //////////////////////////// BUILD ALG_SECTIONS/////////////////////////////
    //printf("line 943\n");
    MovePair alg_sections[alg->length];
    uint8_t numAlgSecs = 0;
    Load_alg_chunks(alg, alg_sections, &numAlgSecs);
    //printf("line 947: numAlgSecs: %hhu\n", numAlgSecs);

    //////////////////////////// SUMMON EDSGAR W. DIJKSTRA ////////////////////////////
    MinHeap* minheap = MinHeap_create(alg_sections, numAlgSecs);
    //printf("line 951\n");
    MinHeapNode* EndNode;
    servoCode_compiler_Dijkstra(minheap, alg_sections, numAlgSecs, INTER_MOVE_TABLE, &EndNode);
    //printf("line 954\n");

    ///////////// Trace and Save Dijkstra Solve //////////////
    DijkstraPath_s Dijkstra = Form_DijkstraPath_from_EndNode(EndNode);
    //printf("line 958\n");
    /////////////////////// Free the heap ////////////////////
    MinHeap_free(minheap);
    //printf("line 961\n");
    //////////////////// Fill in Gaps ///////////////////////
    RobotSolution SOLUTION = Form_RobotSolution_from_DijkstraPath(Dijkstra, INTER_MOVE_TABLE);
    //printf("line 964\n");
    free(Dijkstra.path);
    return SOLUTION;
}