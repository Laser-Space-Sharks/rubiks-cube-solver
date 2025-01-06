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

typedef struct inter_move_table inter_move_table_s;

void print_RobotState(RobotState_s servos);
void print_State(State_s state);
bool compare_states(const State_s* state1, const State_s* state2);

inter_move_table_s* inter_move_table_create();
void inter_move_table_free(inter_move_table_s *ht);
size_t inter_move_table_hash(const RobotState_s *key);

size_t total_nodes_from_alg_secs(MovePair* alg_sections, uint8_t numAlgSecs);
RobotSolution servoCode_compiler_Ofastest(const alg_s* alg, const inter_move_table_s* INTER_MOVE_TABLE);

#endif // SERVOCODER_H