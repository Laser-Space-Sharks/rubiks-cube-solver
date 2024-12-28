#include "servoCoder.h"
#include "main.h"
#include "inter_move_table.h"
#include "alg.h"
#include "move.h"
#include "MinHeap.h"

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
static const face_e ROT_TRAINS[6][4] {
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
    RobotState_s servos = state.servos;
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

typedef struct {
    move_s move1;
    move_s move2;
} MovePair;
typedef struct RobotSolution {
    RobotState_s* solution;
    size_t size;
} RobotSolution;
RobotSolution servoCode_compiler_Ofastest(const alg_s* alg, const inter_move_table_s* INTER_MOVE_TABLE) {
    /////////////////////////////////  LOAD ALG_CHUNKS  //////////////////////////////
    MovePair alg_sections[alg->length];
    uint8_t numAlgSecs = 0;
    for (int i = 0; i < alg->length; i++) {
        move_s movestruct = move_as_struct(alg->moves[i]);
        if (numAlgSecs != 0 && alg_sections[numAlgSecs-1].move1.face == opposite_faces[movestruct.face]) {
            alg_sections[numAlgSecs-1].move2 = movestruct;
        } else {
            alg_sections[numAlgSecs] = (MovePair) {
                .move1 = movestruct,
                .move2 = NULL_MOVE
            }; numAlgSecs++;
        }
    }
    
    //////////////////////////// SUMMON EDSGAR W. DIJKSTRA ////////////////////////////
/*
    ############################################ SUMMON Edsger W. Dijkstra ###############################################
    def update_key(priority_queue_nodes, priority_queue_nums, parent, neighbor, dist):
        if neighbor not in distances:
            p = bisect_left(priority_queue_nums, dist)
            priority_queue_nodes.insert(p, neighbor)
            priority_queue_nums.insert(p, dist)
            distances[neighbor] = dist
            parents[neighbor] = parent

        elif dist < distances[neighbor]:
            p1 = priority_queue_nodes.index(neighbor)
            p2 = bisect_left(priority_queue_nums, dist)
            priority_queue_nodes.pop(p1)
            priority_queue_nodes.insert(p2, neighbor)
            priority_queue_nums.pop(p1)
            priority_queue_nums.insert(p2, dist)
            distances[neighbor] = dist
            parents[neighbor] = parent


    Default_Persp = Orientation('F', 0)

    distances = {Robot_start_state: 0}
    parents = {Robot_start_state: None}
    priority_queue_nodes = [Robot_start_state]
    priority_queue_nums = [0]
    
    Robot_end_state = None
    starting_index = 0
    record_depth = 0

    while starting_index < len(priority_queue_nodes):
        current_node = priority_queue_nodes[starting_index]
        if current_node == Robot_start_state:
            for weight, neighbor0, _ in INTER_MOVE_TABLE[Robot_start_state]:
                if ((len(alg_sections[0]) == 1 and state_can_do_move(alg_sections[0][0], neighbor0)) or 
                    (len(alg_sections[0]) == 2 and state_can_do_opposite_move_pair(alg_sections[0][0], alg_sections[0][1], neighbor0))): 
                    neighbor = (0, 'before', neighbor0)
                    dist = weight
                    update_key(priority_queue_nodes, priority_queue_nums, Robot_start_state, neighbor, dist)
            if ((len(alg_sections[0]) == 1 and state_can_do_move(alg_sections[0][0], Robot_start_state)) or 
                (len(alg_sections[0]) == 2 and state_can_do_opposite_move_pair(alg_sections[0][0], alg_sections[0][1], Robot_start_state))): 
                if len(alg_sections[0]) == 1: 
                    for state in state_after_move(alg_sections[0][0], Robot_start_state):
                        neighbor = (0, 'after', state)
                        dist = calc_weight_of_step(Robot_start_state, state)
                        update_key(priority_queue_nodes, priority_queue_nums, Robot_start_state, neighbor, dist)
                if len(alg_sections[0]) == 2:
                    state = state_after_opposite_moves_pair(alg_sections[0][0], alg_sections[0][1], Robot_start_state)[0]
                    neighbor = (0, 'after', state)
                    dist = calc_weight_of_step(Robot_start_state, state)
                    update_key(priority_queue_nodes, priority_queue_nums, Robot_start_state, neighbor, dist)
        elif current_node[1] == "before":
            N = current_node[0]
            
            if len(alg_sections[N]) == 1: 
                for state in state_after_move(alg_sections[N][0], current_node[2]):
                    neighbor = (N, 'after', state)
                    dist = distances[current_node] + calc_weight_of_step(current_node[2], state)
                    update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor, dist)
            else: 
                state = state_after_opposite_moves_pair(alg_sections[N][0], alg_sections[N][1], current_node[2])[0]
                neighbor = (N, 'after', state)
                dist = distances[current_node] + calc_weight_of_step(current_node[2], state)
                update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor, dist)
        elif current_node[1] == "after":
            N = current_node[0]
            if (N == len(alg_sections)-1): 
                Robot_end_state = current_node
                print(f"WE SOLVED IT!!!! DISTANCE: {distances[current_node]}")
                break
            if (N == record_depth):
                print(f"WE GOT TO THE END OF MOVE {record_depth}")
                record_depth += 1
    
            for weight, neighbor0, _ in INTER_MOVE_TABLE[State(Default_Persp, current_node[2].servos)]:
                new_persp = arr6_to_Orientation[Multiply_arr6s(Orientation_to_arr6[current_node[2].persp], Orientation_to_arr6[neighbor0.persp])]
                neighbor = State(new_persp, neighbor0.servos)
                if ((len(alg_sections[N+1]) == 1 and state_can_do_move(alg_sections[N+1][0], neighbor)) or 
                    (len(alg_sections[N+1]) == 2 and state_can_do_opposite_move_pair(alg_sections[N+1][0], alg_sections[N+1][1], neighbor))): 
                    neighbor2 = (N+1, 'before', neighbor)
                    dist = distances[current_node] + weight
                    update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor2, dist)
            if ((len(alg_sections[N+1]) == 1 and state_can_do_move(alg_sections[N+1][0], current_node[2])) or 
                (len(alg_sections[N+1]) == 2 and state_can_do_opposite_move_pair(alg_sections[N+1][0], alg_sections[N+1][1], current_node[2]))): 
                if len(alg_sections[N+1]) == 1: 
                    for state in state_after_move(alg_sections[N+1][0], current_node[2]):
                        neighbor = (N+1, 'after', state)
                        dist = distances[current_node] + calc_weight_of_step(current_node[2], state)
                        update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor, dist)
                else: 
                    state = state_after_opposite_moves_pair(alg_sections[N+1][0], alg_sections[N+1][1], current_node[2])[0]
                    neighbor = (N+1, 'after', state)
                    dist = distances[current_node] + calc_weight_of_step(current_node[2], state)
                    update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor, dist)
        starting_index += 1
*/
    MinHeap* minheap = MinHeap_create(numAlgSecs);
    MinHeap_update_key(minheap, &ROBOT_START_STATE, -1, 0, 0, NULL);
    Orientation_s Default_Persp = {
        .face = FACE_F,
        .rot = 0
    };
    State_s Robot_end_state;
    State_s* stateAfterMove_arr = malloc(5*sizeof(State_s));
    uint8_t stateAfterMove_len;

    MinHeapNode* current_node = MinHeap_pluck_min(minheap);
    while (current_node->algorithm_index != numAlgSecs-1 || current_node->isBefore) {
        if (State_is_ROBOT_START_STATE(&current_node->state) && current_node->algorithm_index == -1) {
            const RSS_entry_s* RSS = inter_move_table_get_RSS(INTER_MOVE_TABLE);
            for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
                const RSS_sub_entry_s* path = &RSS->paths[pathInd];
                if (alg_sections[0].move2.face == NULL_MOVE.face && alg_sections[0].move2.turns == NULL_MOVE.turns) {
                    if (state_can_do_move(alg_sections[0].move1, path->endState)) {
                        MinHeap_update_key(minheap, &path->endState, 0, true, path->weight, current_node);
                    }
                } else if (state_can_do_opposite_move_pair(alg_sections[0].move1, alg_sections[0].move2, path->endState)) {
                    MinHeap_update_key(minheap, &path->endState, 0, true, path->weight, current_node);
                }
            }
            if (alg_sections[0].move2.face == NULL_MOVE.face && alg_sections[0].move2.turns == NULL_MOVE.turns) {
                if (state_can_do_move(alg_sections[0].move1, current_node->state)) {
                    state_after_move(alg_sections[0].move1, current_node->state, stateAfterMove_len, stateAfterMove_arr);
                    for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < stateAfterMove_len; stateAfterMoveInd++) {
                        MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], 0, false, calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
                    }
                }
            } else if (state_can_do_opposite_move_pair(alg_sections[0].move1, alg_sections[0].move2, current_node->state)) {
                state_after_opposite_moves_pair(alg_sections[0].move1, alg_sections[0].move2, current_node->state, stateAfterMove_len, stateAfterMove_arr);
                for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < stateAfterMove_len; stateAfterMoveInd++) {
                    MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], 0, false, calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
                }
            }
        } else if (current_node->isBefore) { // state is immediately before the move
            uint8_t N = current_node->algorithm_index;
            if (alg_sections[N].move2.face == NULL_MOVE.face && alg_sections[N].move2.turns == NULL_MOVE.turns) {
                state_after_move(alg_sections[N].move1, current_node->state, stateAfterMove_len, stateAfterMove_arr);
                for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < stateAfterMove_len; stateAfterMoveInd++) {
                    MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], N, false, current_node->weight + calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
                }
            } else {
                state_after_opposite_moves_pair(alg_sections[N].move1, alg_sections[N].move2, current_node->state, stateAfterMove_len, stateAfterMove_arr);
                for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < stateAfterMove_len; stateAfterMoveInd++) {
                    MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], N, false, current_node->weight + calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
                }
            }
        } else if (!current_node->isBefore) { // state is immediately after the move
            const inter_move_entry_s* entry = inter_move_table_lookup(INTER_MOVE_TABLE, &current_node->state.servos);
            Orientation_arr6_s current_node_arr6 = Arr6_from_Orientation(current_node->state.persp);
            uint8_t N = current_node->algorithm_index;
            for (size_t pathInd = 0; pathInd < entry->length; pathInd++) {
                const sub_entry_s* path = &entry->paths[pathInd];
                State_s true_endState = path->endState;
                true_endState.persp = Orientation_from_Arr6(multiply_arr6s(current_node_arr6, Arr6_from_Orientation(true_endState.persp)));
                if (alg_sections[N+1].move2.face == NULL_MOVE.face && alg_sections[N+1].move2.turns == NULL_MOVE.turns) {
                    if (state_can_do_move(alg_sections[N+1].move1, true_endState)) {
                        MinHeap_update_key(minheap, &true_endState, N+1, true, current_node->weight + path->weight, current_node);
                    }
                } else if (state_can_do_opposite_move_pair(alg_sections[N+1].move1, alg_sections[N+1].move2, true_endState)) {
                    MinHeap_update_key(minheap, &true_endState, N+1, true, current_node->weight + path->weight, current_node);
                }
            } 
            if (alg_sections[N+1].move2.face == NULL_MOVE.face && alg_sections[N+1].move2.turns == NULL_MOVE.turns) {
                if (state_can_do_move(alg_sections[N+1].move1, current_node->state)) {
                    state_after_move(alg_sections[N+1].move1, current_node->state, stateAfterMove_len, stateAfterMove_arr);
                    for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < stateAfterMove_len; stateAfterMoveInd++) {
                        MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], N+1, false, current_node->weight + calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
                    }
                }
            } else if (state_can_do_opposite_move_pair(alg_sections[N+1].move1, alg_sections[N+1].move2, current_node->state)) {
                state_after_opposite_moves_pair(alg_sections[N+1].move1, alg_sections[N+1].move2, current_node->state, stateAfterMove_len, stateAfterMove_arr);
                for (uint8_t stateAfterMoveInd = 0; stateAfterMoveInd < stateAfterMove_len; stateAfterMoveInd++) {
                    MinHeap_update_key(minheap, &stateAfterMove_arr[stateAfterMoveInd], N+1, false, current_node->weight + calc_weight_of_step(&current_node->state, &stateAfterMove_arr[stateAfterMoveInd]), current_node);
                }
            }
        }
        current_node = MinHeap_pluck_min(minheap);
    } free(stateAfterMove_arr);
    ///////////// Trace and Save Dijkstra Solve //////////////
    size_t solve_path_length = 1;
    MinHeapNode* node = current_node;
    while(node->parent != NULL) {
        node = node->parent;
        solve_path_length++;
    }
    MinHeapNode DijkstraPath[solve_path_length];
    size_t index = solve_path_length-1;
    node = current_node;
    DijkstraPath[index] = *node;
    while(node->parent != NULL) {
        index--;
        node = node->parent;
        DijkstraPath[index] = *node;
    }
    /////////////////////// Free the heap ////////////////////
    MinHeap_free(minheap);
    //////////////////// Fill in Gaps ///////////////////////
/*
    e = []
    parent = parents[Robot_end_state]
    while parent != Robot_start_state: 
        e.append(parent)
        parent = parents[parent]
    pathWithGaps = [Robot_start_state] + e[::-1] + [Robot_end_state]


    pathWithoutGaps = [pathWithGaps[0]]
    for i in range(len(pathWithGaps)-1):
        if pathWithGaps[i] == Robot_start_state:
            if pathWithGaps[i+1][1] != "after":
                for weight, node, interpath in INTER_MOVE_TABLE[Robot_start_state]:
                    if node == pathWithGaps[i+1][2] and node != Robot_start_state:
                        pathWithoutGaps.extend(interpath)
                        break
            pathWithoutGaps.append(pathWithGaps[i+1][2])
        elif pathWithGaps[i][1] == 'before':
            pathWithoutGaps.append(pathWithGaps[i+1][2])
        elif pathWithGaps[i][1] == 'after':
            if pathWithGaps[i+1][1] != "after":
                for weight, node, interpath in INTER_MOVE_TABLE[State(Default_Persp, pathWithGaps[i][2].servos)]:
                    new_persp = arr6_to_Orientation[Multiply_arr6s(Orientation_to_arr6[pathWithGaps[i][2].persp], Orientation_to_arr6[node.persp])]
                    node2 = State(new_persp, node.servos)
                    if node2 == pathWithGaps[i+1][2]:
                        for subnode in interpath:
                            new_persp = arr6_to_Orientation[Multiply_arr6s(Orientation_to_arr6[pathWithGaps[i][2].persp], Orientation_to_arr6[subnode.persp])]
                            subnode2 = State(new_persp, subnode.servos)
                            pathWithoutGaps.append(subnode2)
                        break
            pathWithoutGaps.append(pathWithGaps[i+1][2])
    print(len(pathWithoutGaps)-1)
    for node in pathWithoutGaps:
        print(f"{node.servos.asCommand()} : {node.servos.as2B()}")
    return pathWithoutGaps
*/
    RobotState_s* interpaths_paths[solve_path_length] = { NULL };
    size_t interpaths_lengths[solve_path_length] = { 0 };


    for (size_t i = 0; i < solve_path_length-1; i++) {
        if (State_is_ROBOT_START_STATE(&DijkstraPath[i].state) && DijkstraPath[i].algorithm_index == -1) {
            if (DijkstraPath[i+1].isBefore == 1) {
                const RSS_entry_s* RSS = inter_move_table_get_RSS(INTER_MOVE_TABLE);
                for (size_t pathInd = 0; pathInd < RSS->length; pathInd++) {
                    if (compare_states(&DijkstraPath[i+1].state, &RSS->paths[pathInd].endState)) {
                        interpaths_lengths[i] = RSS->paths[pathInd].size;
                        interpaths_paths[i] = (State_s*)malloc((interpaths_lengths[i])*sizeof(State_s));
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
                State_s true_endState = entry->paths[pathInd].endState;
                true_endState.persp = Orientation_from_Arr6(multiply_arr6s(Arr6, nextArr6));
                if (compare_states(&DijkstraPath[i+1].state, &true_endState)) {
                    interpaths_lengths[i] = entry->paths[pathInd].size;
                    interpaths_paths[i] = (State_s*)malloc((interpaths_lengths[i])*sizeof(State_s));
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
    size_t index = 0;
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