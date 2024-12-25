from bisect import bisect_left
from itertools import product
from Robot_values import *
from DataTypes import *
import numpy as np
import sys
sys.setrecursionlimit(100000)
'''
-----------------------------------------------------
Control Panel:                 |        n
    U2    R2    D2    L2   -   |        
    |     |     |     |        |        N
    U1    R1    D1    L1   [   |      [][][]
    |     |     |     |        | w   W[][][]E   e
    U0    R0    D0    L0   -   |      [][][]
                               |        S
    N     E     S     W        |
    |     |     |     |        |        s
    n     e     s     w
-----------------------------------------------------

--------------------------------------------------------------
Robot Actions on cube:    Conditions:
X  : (R+.L-)     (n.E.s.W), (R0.L1)|(R1.L2)
X' : (R-.L+)     (n.E.s.W), (R1.L0)|(R2.L1)

Y  : (U+.D-)     (N.e.S.w), (U0.D1)|(U1.D2)|(U0.D2)|(U1.D1)
Y2 : (U+2.D-2)   (N.e.S.w), (U0.D2)
Y2': (U-2.D+2)   (N.e.S.w), (U2.D0)
Y' : (U-.D+)     (N.e.S.w), (U1.D0)|(U2.D1)|(U2.D0)|(U1.D1)


cU  : (U+)       (N.E.S.W), (R0)|(R2), (L0)|(L2), (U0)|(U1)      2x2x2x3 = 24
cU2 : (U+2)      (N.E.S.W), (R0)|(R2), (L0)|(L2), (U0)           2x2x1x3 = 12
cU2': (U-2)      (N.E.S.W), (R0)|(R2), (L0)|(L2), (U2)           2x2x1x3 = 12
cU' : (U-)       (N.E.S.W), (R0)|(R2), (L0)|(L2), (U1)|(U2)      2x2x2x3 = 24

cR  : (R+)       (N.E.S.W), (U0)|(U2), (D0)|(D2), (R0)|(R1)      2x2x2x3 = 24
cR2 : (R+2)      (N.E.S.W), (U0)|(U2), (D0)|(D2), (R0)           2x2x1x3 = 12
cR2': (R-2)      (N.E.S.W), (U0)|(U2), (D0)|(D2), (R2)           2x2x1x3 = 12
cR' : (R-)       (N.E.S.W), (U0)|(U2), (D0)|(D2), (R1)|(R2)      2x2x2x3 = 24

cD  : (D+)       (E.S.W), (R0)|(R2), (L0)|(L2), (D0)|(D1)        2x2x2x3x2 = 48
cD2 : (D+2)      (E.S.W), (R0)|(R2), (L0)|(L2), (D0)             2x2x1x3x2 = 24
cD2': (D-2)      (E.S.W), (R0)|(R2), (L0)|(L2), (D2)             2x2x1x3x2 = 24
cD' : (D-)       (E.S.W), (R0)|(R2), (L0)|(L2), (D1)|(D2)        2x2x2x3x2 = 48

cL  : (R+)       (N.E.S.W), (U0)|(U2), (D0)|(D2), (L0)|(L1)      2x2x2x3 = 24
cL2 : (R+2)      (N.E.S.W), (U0)|(U2), (D0)|(D2), (L0)           2x2x1x3 = 12
cL2': (R-2)      (N.E.S.W), (U0)|(U2), (D0)|(D2), (L2)           2x2x1x3 = 12
cL' : (R-)       (N.E.S.W), (U0)|(U2), (D0)|(D2), (L1)|(L2)      2x2x2x3 = 24
--------------------------------------------------------------

----------------------------------------------------------
24 orientations: 
                                                           |
    B             R             F             L            |
   LUR (U, 0)    BUF (U, 1)    RUL (U, 2)    FUB (U, 3)    |
    F             L             B             R            |
                                                           |
    U             B             D             F            |
   FRB (R, 0)    URD (R, 1)    BRF (R, 2)    DRU (R, 3)    |
    D             F             U             B            |
                                                           |
    U             R             D             L            |
   LFR (F, 0)    UFD (F, 1)    RFL (F, 2)    DFU (F, 3)    |
    D             L             U             R            |
                                                           |
    U             F             D             B            |
   BLF (L, 0)    ULD (L, 1)    FLB (L, 2)    DLU (L, 3)    |
    D             B             U             F            |
                                                           |
    U             L             D             R            |
   RBL (B, 0)    UBD (B, 1)    LBR (B, 2)    DBU (B, 3)    |
    D             R             U             L            |
                                                           |
    F             R             B             L            |
   LUR (D, 0)    FUB (D, 1)    RUL (D, 2)    BUF (D, 3)    |
    B             L             F             R            |
----------------------------------------------------------
'''

ROT_TRAINS: dict[str, tuple[tuple[str, int]]] = {
    'U': (('B', 2), ('R', 1), ('F', 0), ('L', 3)),
    'R': (('U', 3), ('B', 0), ('D', 1), ('F', 0)),
    'F': (('U', 0), ('R', 0), ('D', 0), ('L', 0)),
    'L': (('U', 1), ('F', 0), ('D', 3), ('B', 0)),
    'B': (('U', 2), ('L', 0), ('D', 2), ('R', 0)),
    'D': (('F', 0), ('R', 3), ('B', 2), ('L', 1)),
}

def reorientation(O: Orientation, move: CubeRotation) -> Orientation:
    if move.rotation == "X":   thing = ROT_TRAINS[O.face][(2+O.rot)%4]; return Orientation(thing[0], (O.rot + thing[1])%4)
    elif move.rotation == "X'":  thing = ROT_TRAINS[O.face][(0+O.rot)%4]; return Orientation(thing[0], (O.rot + thing[1])%4)
    elif move.rotation == "Y":   thing = ROT_TRAINS[O.face][(1+O.rot)%4]; return Orientation(thing[0], (O.rot + thing[1])%4)
    elif move.rotation == "Y2":  thing = ROT_TRAINS[O.face][(1+O.rot)%4]; return reorientation(Orientation(thing[0], (O.rot + thing[1])%4), CubeRotation("Y"))
    elif move.rotation == "Y2'": thing = ROT_TRAINS[O.face][(1+O.rot)%4]; return reorientation(Orientation(thing[0], (O.rot + thing[1])%4), CubeRotation("Y"))
    elif move.rotation == "Y'":  thing = ROT_TRAINS[O.face][(3+O.rot)%4]; return Orientation(thing[0], (O.rot + thing[1])%4)

def move_from_orientation(O: Orientation, move: RoboMove) -> Move:
    return Move(ROT_TRAINS[O.face][(O.rot + move.turns)%4][0], move.turns)

def stateNum_to_state(stateNum) -> State:
    cpy = stateNum
    Lrot = cpy%3; cpy //= 3
    Drot = cpy%3; cpy //= 3
    Rrot = cpy%3; cpy //= 3
    Urot = cpy%3; cpy //= 3
    Le = cpy%2; cpy //= 2
    De = cpy%2; cpy //= 2
    Re = cpy%2; cpy //= 2
    Ue = cpy%2; cpy //= 2
    turns = cpy%4; cpy //= 4
    face = "URFLBD"[cpy%6]; cpy //= 6
    return State(Orientation(face, turns), RobotState(ArmState(Ue, Urot), ArmState(Re, Rrot), ArmState(De, Drot), ArmState(Le, Lrot)))

def calc_weight_of_step(state: State, state2: State) -> float:
    MAX = 0
    for v1, v2 in zip(state.unpackServos(), state2.unpackServos()):
        if (v1.e, v2.e) == (0, 1): MAX = max(MAX, Etime)
        elif (v1.e, v2.e) == (1, 0): MAX = max(MAX, dEtime)
        elif abs(v1.rot - v2.rot) == 1: MAX = max(MAX, rot1time)
        elif abs(v1.rot - v2.rot) == 2: MAX = max(MAX, rot2time)
    return MAX
def calc_weight_of_path(path: list[State]) -> float:
    return sum(calc_weight_of_step(path[i], path[i+1]) for i in range(len(path)-1))

def get_inter_move_table():
    inter_move_table = {}
    with open("ServoOptimizationTable.txt", "r") as file:
        for line in file.readlines():
            path = [stateNum_to_state(int(num)) for num in line.strip().split()]
            if path[0] not in inter_move_table:
                inter_move_table[path[0]] = []
            
            inter_move_table[path[0]].append((calc_weight_of_path(path), path[-1], [path[i] for i in range(1, len(path)-1)]))
        
    return inter_move_table

INTER_MOVE_TABLE: dict[State, list[tuple[float, State, list[State]]]] = get_inter_move_table()

print(f"WE GOT THE INTER_MOVE_TABLE: length {len(INTER_MOVE_TABLE)}")

def state_after_move(move: Move, state: State) -> list[State]|None:
    robotmove = 0
    if move_from_orientation(state.persp, RoboMove('U', 1)).face == move.face: robotmove = RoboMove('U', move.turns)
    elif move_from_orientation(state.persp, RoboMove('R', 1)).face == move.face: robotmove = RoboMove('R', move.turns)
    elif move_from_orientation(state.persp, RoboMove('D', 1)).face == move.face: robotmove = RoboMove('D', move.turns)
    elif move_from_orientation(state.persp, RoboMove('L', 1)).face == move.face: robotmove = RoboMove('L', move.turns)
    else: return None


    if robotmove.face == 'U':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return None
        if state.servos.R.rot == 1 or state.servos.L.rot == 1: return None
        if robotmove.turns == 1:
            if state.servos.U.rot < 2: return [State(state.persp, RobotState(ArmState(1, state.servos.U.rot+1), state.servos.R, state.servos.D, state.servos.L))]
            else: return None
        if robotmove.turns == 2:
            if state.servos.U.rot == 1: return None
            else: return [State(state.persp, RobotState(ArmState(1, 2-state.servos.U.rot), state.servos.R, state.servos.D, state.servos.L))]
        if robotmove.turns == 3:
            if state.servos.U.rot > 0: return [State(state.persp, RobotState(ArmState(1, state.servos.U.rot-1), state.servos.R, state.servos.D, state.servos.L))]
            else: return None
    if robotmove.face == 'R':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return None
        if state.servos.U.rot == 1 or state.servos.D.rot == 1: return None
        if robotmove.turns == 1:
            if state.servos.R.rot < 2: return [State(state.persp, RobotState(state.servos.U, ArmState(1, state.servos.R.rot+1), state.servos.D, state.servos.L))]
            else: return None
        if robotmove.turns == 2:
            if state.servos.R.rot == 1: return None
            else: return [State(state.persp, RobotState(state.servos.U, ArmState(1, 2-state.servos.R.rot), state.servos.D, state.servos.L))]
        if robotmove.turns == 3:
            if state.servos.R.rot > 0: return [State(state.persp, RobotState(state.servos.U, ArmState(1, state.servos.R.rot-1), state.servos.D, state.servos.L))]
            else: return None
    if robotmove.face == 'L':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return None
        if state.servos.U.rot == 1 or state.servos.D.rot == 1: return None
        if robotmove.turns == 1:
            if state.servos.L.rot < 2: return [State(state.persp, RobotState(state.servos.U, state.servos.R, state.servos.D, ArmState(1, state.servos.L.rot+1)))]
            else: return None
        if robotmove.turns == 2:
            if state.servos.L.rot == 1: return None
            else: return [State(state.persp, RobotState(state.servos.U, state.servos.R, state.servos.D, ArmState(1, 2-state.servos.L.rot)))]
        if robotmove.turns == 3:
            if state.servos.L.rot > 0: return [State(state.persp, RobotState(state.servos.U, state.servos.R, state.servos.D, ArmState(1, state.servos.L.rot-1)))]
            else: return None
    if robotmove.face == 'D':
        if not (state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return None
        if state.servos.R.rot == 1 or state.servos.L.rot == 1: return None
        endStateOfD = None
        if robotmove.turns == 1:
            if state.servos.L.rot < 2: endStateOfD = ArmState(1, state.servos.D.rot+1)
            else: return None
        if robotmove.turns == 2:
            if state.servos.L.rot == 1: return None
            else: endStateOfD = ArmState(1, 2-state.servos.D.rot)
        if robotmove.turns == 3:
            if state.servos.L.rot > 0: endStateOfD = ArmState(1, state.servos.L.rot-1)
            else: return None
        
        if (state.servos.U.e):
            return [State(state.persp, RobotState(state.servos.U, state.servos.R, endStateOfD, state.servos.L)),
                    State(state.persp, RobotState(ArmState(0, state.servos.U.rot), state.servos.R, endStateOfD, state.servos.L))]
        else:
            return [State(state.persp, RobotState(ArmState(1, state.servos.U.rot), state.servos.R, endStateOfD, state.servos.L)),
                    State(state.persp, RobotState(ArmState(0, 0), state.servos.R, endStateOfD, state.servos.L)),
                    State(state.persp, RobotState(ArmState(0, 1), state.servos.R, endStateOfD, state.servos.L)),
                    State(state.persp, RobotState(ArmState(0, 2), state.servos.R, endStateOfD, state.servos.L))]
def state_after_opposite_moves_pair(move1: Move, move2: Move, state: State):
    if move1.face != opposites[move2.face]: return None
    if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return None

    if move1.face == 'D':
        temp = state_after_move(move1, state)
        if temp is None: return None
        else: return state_after_move(move2, temp[0])
    if move2.face == 'D':
        temp = state_after_move(move2, state)
        if temp is None: return None
        else: return state_after_move(move1, temp[0])
    
    temp = state_after_move(move1, state)
    if temp is None: return None
    else: return state_after_move(move2, temp[0])

def algstr_to_alg(algstr: str):
    return [
        (mv[0], {"'": 3, "2": 2}.get(mv[-1], 1))
        for mv in algstr.strip().split()
    ]

def Optimize_for_alg(alg: list[tuple[str, int]]):
    alg_sections = [[Move(alg[0][0], alg[0][1])]]
    for mv in alg:
        if len(alg_sections[-1]) == 1 and alg_sections[-1][0].face == opposites[mv[0]]:
            alg_sections[-1].append(Move(mv[0], mv[1]))
        else: alg_sections.append([Move(mv[0], mv[1])])

    ############################################ SUMMON Edsger W. Dijkstra ###############################################
    def update_key(priority_queue_nodes, priority_queue_nums, parent, neighbor, dist):
        if neighbor not in distances:
            p = bisect_left(priority_queue_nums, dist)+1
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
    
    solved = False
    starting_index = 0
    record_depth = 0

    while starting_index < len(priority_queue_nodes):
        current_node = priority_queue_nodes[starting_index]
        if current_node == Robot_start_state:
            for weight, neighbor0, _ in INTER_MOVE_TABLE[Robot_start_state]:
                statesaftermove = None
                if len(alg_sections[0]) == 1: 
                    statesaftermove = state_after_move(alg_sections[0][0], neighbor0)
                    if statesaftermove is None: continue
                else: 
                    statesaftermove = state_after_opposite_moves_pair(alg_sections[0][0], alg_sections[0][1], neighbor0)
                    if statesaftermove is None: continue
                
                neighbor = (0, 'before', neighbor0)
                dist = weight
                update_key(priority_queue_nodes, priority_queue_nums, Robot_start_state, neighbor, dist)
        elif current_node[1] == "before":
            N = current_node[0]
            statesaftermove = None
            if len(alg_sections[N]) == 1: 
                statesaftermove = state_after_move(alg_sections[N][0], current_node[2])
                if statesaftermove is None: continue
            else: 
                statesaftermove = state_after_opposite_moves_pair(alg_sections[N][0], alg_sections[N][1], current_node[2])
                if statesaftermove is None: continue
            for state in statesaftermove:
                neighbor = (N, 'after', state)
                dist = distances[current_node] + calc_weight_of_step(current_node[2], state)
                update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor, dist)
        elif current_node[1] == "after":
            N = current_node[0]
            if (N == len(alg_sections)-1): 
                solved = True
                print(f"WE SOLVED IT!!!! DISTANCE: {distances[current_node]}")
                break
            if (N == record_depth):
                print(f"WE GOT TO THE END OF MOVE {record_depth}")
                record_depth += 1
            for weight, neighbor0, _ in INTER_MOVE_TABLE[State(Default_Persp, current_node[2].servos)]:
                new_persp = arr6_to_Orientation[Multiply_arr6s(Orientation_to_arr6[current_node[2].persp], Orientation_to_arr6[neighbor0.persp])]
                neighbor = State(new_persp, neighbor0.servos)
                statesaftermove = None
                if len(alg_sections[N+1]) == 1: 
                    statesaftermove = state_after_move(alg_sections[N+1][0], neighbor)
                    if statesaftermove is None: continue
                else: 
                    statesaftermove = state_after_opposite_moves_pair(alg_sections[N+1][0], alg_sections[N+1][1], neighbor)
                    if statesaftermove is None: continue
                
                neighbor2 = (N+1, 'before', neighbor)
                dist = distances[current_node] + weight
                update_key(priority_queue_nodes, priority_queue_nums, current_node, neighbor2, dist)
        starting_index += 1
    #########################################################################################################
    
    def trace_path(state1: State, parents: dict[State, State], state2):
        e = []
        parent = parents[state2]
        while parent != state1: 
            e.append(parent)
            parent = parents[parent]
        return [state1] + e[::-1] + [state2]

Optimize_for_alg(algstr_to_alg("F' D' L2 R U' R"))
