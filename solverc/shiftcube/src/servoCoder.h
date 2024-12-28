#ifndef SERVOCODER_H
#define SERVOCODER_H

#include "main.h"

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
typedef struct {
    Orientation_s persp;
    RobotState_s servos;
} State_s; // 4 Bytes

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

State_s stateNum_to_state(uint16_t stateNum);
bool compare_RobotStates(const RobotState_s* state1, const RobotState_s* state2);
bool State_is_ROBOT_START_STATE(const State_s* state);
bool compare_states(const State_s* state1, const State_s* state2);
double calc_weight_of_step(const State_s* state1, const State_s* state2);

#endif // SERVOCODER_H

