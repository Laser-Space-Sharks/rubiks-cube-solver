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

typedef struct {
    State_s endState;
    double weight;
    State_s* path;
    size_t size;
} RSS_sub_entry_s;
typedef struct {
    State_s startState;
    RSS_sub_entry_s* paths;
    size_t length;
    size_t size;
} RSS_entry_s;

typedef struct {
    State_s endState;
    double weight;
    RobotState_s* path;
    size_t size;
} sub_entry_s;
typedef struct {
    RobotState_s startState;
    sub_entry_s* paths;
    size_t length;
    size_t size;
} inter_move_entry_s;

typedef struct inter_move_table inter_move_table_s;

inter_move_table_s* inter_move_table_create();
size_t inter_move_table_hash(const RobotState_s *key);
const RSS_entry_s* inter_move_table_get_RSS(const inter_move_table_s *ht);
const inter_move_entry_s* inter_move_table_lookup(const inter_move_table_s *ht, const RobotState_s *key);
void inter_move_table_free(inter_move_table_s *ht);

void print_RobotState(RobotState_s state);
void print_State(State_s state);
State_s stateNum_to_state(uint16_t stateNum);
bool compare_RobotStates(const RobotState_s* state1, const RobotState_s* state2);
bool State_is_ROBOT_START_STATE(const State_s* state);
bool compare_states(const State_s* state1, const State_s* state2);
double calc_weight_of_step(const State_s* state1, const State_s* state2);
RobotSolution servoCode_compiler_Ofastest(const alg_s* alg, const inter_move_table_s* INTER_MOVE_TABLE);

#endif // SERVOCODER_H