#ifndef SERVOCODER_H
#define SERVOCODER_H

#include "main.h"
#include "alg.h"

typedef struct {
    face_e face;
    side_e rot;
} Orientation_s; // 2 Bytes
typedef struct {
    uint16_t n : 1;
    uint16_t e : 1;
    uint16_t s : 1;
    uint16_t w : 1;
    uint16_t U : 2;
    uint16_t R : 2;
    uint16_t D : 2;
    uint16_t L : 2;
} RobotState_s; // 2 Bytes
typedef struct State_s {
    Orientation_s persp;
    RobotState_s servos;
} State_s; // 4 Bytes

typedef struct RobotSolution {
    RobotState_s* solution;
    size_t size;
} RobotSolution;

static const State_s ROBOT_START_STATE = {
    .persp = (Orientation_s) {
        FACE_F, 0
    },
    .servos = (RobotState_s) {
        1,1,1,1,0,0,0,0
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

typedef struct {                                                  // There will be 735 of these lying around
    State_s endState; // 4 bytes
    uint32_t singleMoveQualifications; // 4 bytes
    float weight; // 4 bytes
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
    float weight; // 4 bytes
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

inter_move_table_s* inter_move_table_create();
size_t inter_move_table_hash(const RobotState_s *key);
const RSS_entry_s* inter_move_table_get_RSS(const inter_move_table_s *ht);
const inter_move_entry_s* inter_move_table_lookup(const inter_move_table_s *ht, const RobotState_s *key);
void inter_move_table_free(inter_move_table_s *ht);

bool state_can_do_move(move_s move, State_s state);
void print_RobotState(RobotState_s state);
void print_State(State_s state);
State_s stateNum_to_state(uint16_t stateNum);
bool compare_RobotStates(const RobotState_s* state1, const RobotState_s* state2);
bool State_is_ROBOT_START_STATE(const State_s* state);
bool compare_states(const State_s* state1, const State_s* state2);
float calc_weight_of_step(const State_s* state1, const State_s* state2);
RobotSolution servoCode_compiler_Ofastest(const alg_s* alg, const inter_move_table_s* INTER_MOVE_TABLE);

#endif // SERVOCODER_H