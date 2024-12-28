#ifndef SERVOCODER_H
#define SERVOCODER_H

#include "main.h"
#include "alg.h"

#define Etime 1
#define dEtime 1
#define rot1time 1
#define rot2time 1

typedef struct {
    face_e face;
    side_e rot;
} Orientation_s; // 2 Bytes

typedef struct {
    face_e faces[6];
} Orientation_arr6_s; // 6 Bytes

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

static const State_s NULL_STATE = {
    .persp = {
        .face = FACE_NULL,
        .rot = 0
    },
    .servos = {
        0,0,0,0,0,0,0,0
    }
};

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

static const State_s ROBOT_START_STATE = {
    .persp = (Orientation_s) {
        FACE_F, 0
    },
    .servos = (RobotState_s) {
        1,1,1,1,0,0,0,0
    }
};

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
double calc_weight_of_armstep(bool e1, uint8_t rot1, bool e2, uint8_t rot2) {
    if (e1 == 0 && e2 == 1) return (double)Etime;
    if (e1 == 1 && e2 == 0) return (double)dEtime;
    if (abs(rot2 - rot1) == 1) return (double)rot1time;
    if (abs(rot2 - rot1) == 2) return (double)rot2time;
}
double calc_weight_of_step(const State_s* state1, const State_s* state2) {
    double maxWeight = 0;
    double arm1 = calc_weight_of_armstep(state1->servos.n, state1->servos.U, state2->servos.n, state2->servos.U);
    double arm2 = calc_weight_of_armstep(state1->servos.e, state1->servos.R, state2->servos.e, state2->servos.R);
    double arm3 = calc_weight_of_armstep(state1->servos.s, state1->servos.D, state2->servos.s, state2->servos.D);
    double arm4 = calc_weight_of_armstep(state1->servos.w, state1->servos.L, state2->servos.w, state2->servos.L);
    maxWeight = (maxWeight > arm1) ? maxWeight : arm1;
    maxWeight = (maxWeight > arm2) ? maxWeight : arm2;
    maxWeight = (maxWeight > arm3) ? maxWeight : arm3;
    maxWeight = (maxWeight > arm4) ? maxWeight : arm4;
    return maxWeight;
}

#endif // SERVOCODER_H

