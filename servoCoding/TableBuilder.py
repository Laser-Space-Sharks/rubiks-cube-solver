from bisect import bisect_left
from dataclasses import dataclass
from itertools import product
import numpy as np
import sys
from Robot_values import *
from DataTypes import *
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

#######################################################################################################
######                                                                                           ######
######                                   CREATE TOTAL WEIGHTED GRAPH                             ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
ALLPERSPS = [Orientation(face, turns) for face, turns in product("URFLBD", range(4))]
UNEXTENDED_ARMS = [ArmState(0, 0), ArmState(0, 1), ArmState(0, 2)]
EXTENDED_ARMS = [ArmState(1, 0), ArmState(1, 1), ArmState(1, 2)]
ALL_ARM_STATES = [ArmState(0, 0), ArmState(0, 1), ArmState(0, 2),
                ArmState(1, 0), ArmState(1, 1), ArmState(1, 2)]

def edge_exists(startState, endState, TOTAL_STATES):
    if startState not in TOTAL_STATES: return False
    for _, state in TOTAL_STATES[startState]:
        if state == endState: return True
    return False

#BADSTART = State(Orientation('D', 3), RobotState(ArmState(1, 2), ArmState(1, 2), ArmState(1, 2), ArmState(1, 2)))
#BADEND = State(Orientation('R', 1), RobotState(ArmState(1, 2), ArmState(0, 2), ArmState(1, 1), ArmState(0, 1)))
#########################################################
##                      FUNCTIONS                      ##
#########################################################
def is_valid_state(state: State):
    U, R, D, L = state.servos.U, state.servos.R, state.servos.D, state.servos.L
    if R == ArmState(1, 1) == D: return False
    if L == ArmState(1, 1) == D: return False
    if R == ArmState(1, 1) == U: return False
    if L == ArmState(1, 1) == U: return False
    return (D.e == 1 or (R == ArmState(1, 1) and L.e == 1) or (L == ArmState(1, 1) and R.e == 1))
def y1(state: State):
    cubeRot = CubeRotation("Y")
    persp, U, R, D, L = state.unpack()
    a = set()
    if (R.e, L.e, U.e, D.e) == (0, 0, 1, 1) and  U.rot < 2 and D.rot > 0:
        for R2, L2 in product(UNEXTENDED_ARMS, UNEXTENDED_ARMS):
            a.add(State(reorientation(persp, cubeRot), RobotState(ArmState(1, U.rot+1), R2, ArmState(1, D.rot-1), L2)))
    return a
def y3(state: State):
    cubeRot = CubeRotation("Y'")
    persp, U, R, D, L = state.unpack()
    a = set()
    if (R.e, L.e, U.e, D.e) == (0, 0, 1, 1) and  U.rot > 0 and D.rot < 2:
        for R2, L2 in product(UNEXTENDED_ARMS, UNEXTENDED_ARMS):
            a.add(State(reorientation(persp, cubeRot), RobotState(ArmState(1, U.rot-1), R2, ArmState(1, D.rot+1), L2)))
    return a
def y2(state: State):
    cubeRot = CubeRotation("Y2")
    persp, U, R, D, L = state.unpack()
    a = set()
    if (R.e, L.e, U.e, D.e) == (0, 0, 1, 1) and abs(U.rot - D.rot) == 2:
        for R2, L2 in product(UNEXTENDED_ARMS, UNEXTENDED_ARMS):
            a.add(State(reorientation(persp, cubeRot), RobotState(D, R2, U, L2)))
    return a
def x1(state: State):
    cubeRot = CubeRotation("X")
    persp, U, R, D, L = state.unpack()
    a = set()
    if (R.e, L.e, U.e, D.e) == (1, 1, 0, 0) and (L.rot - R.rot == 1):
        for U2, D2 in product(UNEXTENDED_ARMS, UNEXTENDED_ARMS):
            a.add(State(reorientation(persp, cubeRot), RobotState(U2, L, D2, R)))
    return a
def x3(state: State):
    cubeRot = CubeRotation("X'")
    persp, U, R, D, L = state.unpack()
    a = set()
    if (R.e, L.e, U.e, D.e) == (1, 1, 0, 0) and (R.rot - L.rot == 1):
        for U2, D2 in product(UNEXTENDED_ARMS, UNEXTENDED_ARMS):
            a.add(State(reorientation(persp, cubeRot), RobotState(U2, L, D2, R)))
    return a
def calc_weight(state: State, state2: State):
    MAX = 0
    for v1, v2 in zip(state.unpackServos(), state2.unpackServos()):
        if (v1.e, v2.e) == (0, 1): MAX = max(MAX, Etime)
        elif (v1.e, v2.e) == (1, 0): MAX = max(MAX, dEtime)
        elif abs(v1.rot - v2.rot) == 1: MAX = max(MAX, rot1time)
        elif abs(v1.rot - v2.rot) == 2: MAX = max(MAX, rot2time)
    return MAX
def is_valid_step(state: State, state2: State):
    if state == state2: return False
    U, R, D, L = state.unpackServos()
    U2, R2, D2, L2 = state2.unpackServos()
    for v1, v2 in zip(state.unpackServos(), state2.unpackServos()):
        if v1.rot != v2.rot and (v1.e or v2.e): return False

    Etoggling = [(U.e!=U2.e), (R.e!=R2.e), (D.e!=D2.e), (L.e!=L2.e)]
    if Etoggling[0] and Etoggling[1] and U.rot == R.rot == 1: return False
    if Etoggling[1] and Etoggling[2] and R.rot == D.rot == 1: return False
    if Etoggling[2] and Etoggling[3] and D.rot == L.rot == 1: return False
    if Etoggling[3] and Etoggling[0] and L.rot == U.rot == 1: return False
    if Etoggling[2] and (Etoggling[1] or Etoggling[3]): return False
    return True
#########################################################
##                         GO                          ##
#########################################################
TOTAL_STATES: dict[State, list[tuple[float, State]]] = {}

for persp, armU, armR, armD, armL in product(ALLPERSPS, ALL_ARM_STATES, ALL_ARM_STATES, ALL_ARM_STATES, ALL_ARM_STATES):
    state = State(persp, RobotState(armU, armR, armD, armL))
    if is_valid_state(state):
        TOTAL_STATES[state] = []
# y1
for persp, armU, armR, armD, armL in product(ALLPERSPS, [ArmState(1, 1), ArmState(1, 0)],
                                                        UNEXTENDED_ARMS,
                                                        [ArmState(1, 2), ArmState(1, 1)],
                                                        UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in y1(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), Y1time), endState))
#    if edge_exists(BADSTART, BADEND, TOTAL_STATES):
#        print(BADSTART)
#        print(startState)
#        exit()
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
#print(y1(BADSTART))
# y2
for persp, armR, armL, (armU, armD) in product(ALLPERSPS, UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS,
                                                          [(ArmState(1, 0), ArmState(1, 2)), (ArmState(1, 2), ArmState(1, 0))]):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in y2(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), Y2time), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
# y3
for persp, armU, armR, armD, armL in product(ALLPERSPS, [ArmState(1, 1), ArmState(1, 2)],
                                                        UNEXTENDED_ARMS,
                                                        [ArmState(1, 0), ArmState(1, 1)],
                                                        UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in y3(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), Y3time), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
# x1
for persp, (armR, armL), armU, armD in product(ALLPERSPS, [(ArmState(1, 1), ArmState(1, 2)), (ArmState(1, 0), ArmState(1, 1))],
                                                          UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in x1(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), X1time), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
# x3
for persp, (armR, armL), armU, armD in product(ALLPERSPS, [(ArmState(1, 1), ArmState(1, 0)), (ArmState(1, 2), ArmState(1, 1))],
                                                          UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in x3(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), X3time), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
'''
Before, We created empty nodes to address every possible servo state.
We then connected all the nodes we could with all the cube rotations we could.
Now, we will finish by focusing on no cube rotations, and permutating the possible and valid individual arm movements.
'''
for startState in TOTAL_STATES.keys():
    ArmOptions = []
    for arm in startState.unpackServos():
        if arm.e: ArmOptions.append([ArmState(0, arm.rot), ArmState(1, arm.rot)]) # If extended, can only detract
        else: ArmOptions.append([ArmState(1, arm.rot)] + UNEXTENDED_ARMS) # Else, can extend or rotate

    for armU, armR, armD, armL in product(*ArmOptions):
        endState = State(startState.persp, RobotState(armU, armR, armD, armL))
        if is_valid_state(endState) and is_valid_step(startState, endState):
            TOTAL_STATES[startState].append((calc_weight(startState, endState), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
print(f"# of possible servo states: {len(list(TOTAL_STATES.keys()))}")
print(f"# of edges in graph: {sum(len(i) for i in TOTAL_STATES.values())/2}")
print(f"-------------------------------------")

ROBOT_MOVE_STATES = set()
# Get everything that can move U and D
for armU, armR, armD, armL in product(EXTENDED_ARMS, 
                                      [ArmState(1, 0), ArmState(1, 2)],
                                      EXTENDED_ARMS,
                                      [ArmState(1, 0), ArmState(1, 2)]):
    state = RobotState(armU, armR, armD, armL)
    ROBOT_MOVE_STATES.add(state)
# Then get everything else that can move D
for armU, armR, armD, armL in product(UNEXTENDED_ARMS, 
                                      [ArmState(1, 0), ArmState(1, 2)],
                                      EXTENDED_ARMS,
                                      [ArmState(1, 0), ArmState(1, 2)]):
    state = RobotState(armU, armR, armD, armL)
    ROBOT_MOVE_STATES.add(state)
# Then get everything that can move R and L
for armU, armR, armD, armL in product([ArmState(1, 0), ArmState(1, 2)],
                                      EXTENDED_ARMS, 
                                      [ArmState(1, 0), ArmState(1, 2)],
                                      EXTENDED_ARMS):
    state = RobotState(armU, armR, armD, armL)
    ROBOT_MOVE_STATES.add(state)

print(len(ROBOT_MOVE_STATES))



#######################################################################################################
######                                                                                           ######
######                                         ANALYZE GRAPH                                     ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
#########################################################
##                      FUNCTIONS                      ##
#########################################################
def check_connection(node, already_checked: set[State]):
    for _, node2 in TOTAL_STATES[node]:
        if node2 not in already_checked:
            already_checked.add(node2)
            check_connection(node2, already_checked)
#########################################################
##                         GO                          ##
#########################################################
# We will now separate the total graph into its separate connected components, if there are any.
CONNECTED_COMPONENTS = []
for i in TOTAL_STATES:
    good = False
    for space in CONNECTED_COMPONENTS:
        if i in space:
            good = True
            break
    if not good:
        newspace = set()
        check_connection(i, newspace)
        CONNECTED_COMPONENTS.append(newspace)

SEPARATED_TOTAL_STATES = [{key: TOTAL_STATES[key] for key in Connected_component} for Connected_component in CONNECTED_COMPONENTS]

print(f"There are {len(CONNECTED_COMPONENTS)} separate connected components in the graph")
for i, v in enumerate(CONNECTED_COMPONENTS):
    print(f"nodes in servospace{i+1}: {len(v)}")
for i, STATES in enumerate(SEPARATED_TOTAL_STATES):
    for v in STATES.values():
        for w, node in v:
            if node not in STATES:
                print(f"{node} not in SEPARATED_TOTAL_STATES[{i}]")

print(f"-------------------------------------")

#######################################################################################################
######                                                                                           ######
######                                         USE DIJKSTRA                                      ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
def Dijkstra(source: State, graph_dict: dict[State, list[float, State]]):
    distances = {key: np.inf for key in graph_dict.keys()}
    distances[source] = 0
    parents = {source: None}
    l = len(list(graph_dict.keys()))
    priority_queue_nodes = [source]
    for i in graph_dict.keys(): 
        if i != source: priority_queue_nodes.append(i)
    priority_queue_nums = [0]
    for i in graph_dict.keys(): 
        if i != source: priority_queue_nums.append(distances[i])
    for starting_index in range(l):
        current_node = priority_queue_nodes[starting_index]
        for weight, neighbor in graph_dict[current_node]:
            dist = weight + distances[current_node]
            if dist < distances[neighbor]:
                p1 = priority_queue_nodes.index(neighbor)
                p2 = bisect_left(priority_queue_nums, dist)
                priority_queue_nodes.pop(p1)
                priority_queue_nodes.insert(p2, neighbor)
                priority_queue_nums.pop(p1)
                priority_queue_nums.insert(p2, dist)
                distances[neighbor] = dist
                parents[neighbor] = current_node
    return distances, parents
def trace_path(state1: State, parents: dict[State, State], state2):
    e = []
    parent = parents[state2]
    while parent != state1: 
        e.append(parent)
        parent = parents[parent]
    return [state1] + e[::-1] + [state2]



def Dijkstra_to_all_MOVE_STATES(startStates: State, paths):
    count = 0
    for startState in startStates:
        for Connected_graph in SEPARATED_TOTAL_STATES:
            if startState not in Connected_graph: continue
            _, parents = Dijkstra(startState, Connected_graph)
            count += 1
            print(f"\rDijkstra finished! {count}/{len(ROBOT_MOVE_STATES)}", end='', flush=True)
            for persp, Rstate2 in product(ALLPERSPS, ROBOT_MOVE_STATES):
                endState = State(persp, Rstate2)
                if endState == startState: continue
                if endState not in Connected_graph: continue
                paths.append(tuple(trace_path(startState, parents, endState)))
            break
    print()


PATHS = []
states = [State(Orientation('F', 0), Rstate1) for Rstate1 in ROBOT_MOVE_STATES]

Dijkstra_to_all_MOVE_STATES(states, PATHS)
ROOT_PATHS = []
Dijkstra_to_all_MOVE_STATES([Robot_start_state], ROOT_PATHS)

print(len(PATHS))
print(len(ROOT_PATHS))

with open("ServoOptimizationTable.txt", "w") as file:
    for path in PATHS:
        line = ' '.join([str(i.asNum()) for i in path])
        file.write(f"{line}\n")
with open("ServoOptimizationTable_rootpaths.txt", "w") as file:
    for path in ROOT_PATHS:
        line = ' '.join([str(i.asNum()) for i in path])
        file.write(f"{line}\n")