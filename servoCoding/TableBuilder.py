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
    return (D.e or (R == ArmState(1, 1) and L.e) or (L == ArmState(1, 1) and R.e))
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
def calc_action(state: State, state2: State):
    SUM = 0
    for v1, v2 in zip(state.unpackServos(), state2.unpackServos()):
        if (v1.e, v2.e) == (0, 1): SUM += Etime
        elif (v1.e, v2.e) == (1, 0): SUM += dEtime
        elif abs(v1.rot - v2.rot) == 1: SUM += rot1time
        elif abs(v1.rot - v2.rot) == 2: SUM += rot2time
    return SUM
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
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), Y1time), calc_action(startState, endState), endState))
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
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), Y2time), calc_action(startState, endState), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
# y3
for persp, armU, armR, armD, armL in product(ALLPERSPS, [ArmState(1, 1), ArmState(1, 2)],
                                                        UNEXTENDED_ARMS,
                                                        [ArmState(1, 0), ArmState(1, 1)],
                                                        UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in y3(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), Y3time), calc_action(startState, endState), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
# x1
for persp, (armR, armL), armU, armD in product(ALLPERSPS, [(ArmState(1, 1), ArmState(1, 2)), (ArmState(1, 0), ArmState(1, 1))],
                                                          UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in x1(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), X1time), calc_action(startState, endState), endState))
#print(edge_exists(BADSTART, BADEND, TOTAL_STATES))
# x3
for persp, (armR, armL), armU, armD in product(ALLPERSPS, [(ArmState(1, 1), ArmState(1, 0)), (ArmState(1, 2), ArmState(1, 1))],
                                                          UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in x3(startState):
        TOTAL_STATES[startState].append((max(calc_weight(startState, endState), X3time), calc_action(startState, endState), endState))
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
            TOTAL_STATES[startState].append((calc_weight(startState, endState), calc_action(startState, endState), endState))
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
    for _, _, node2 in TOTAL_STATES[node]:
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
        for w, a, node in v:
            if node not in STATES:
                print(f"{node} not in SEPARATED_TOTAL_STATES[{i}]")

print(f"-------------------------------------")

#######################################################################################################
######                                                                                           ######
######                                         USE DIJKSTRA                                      ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
@dataclass()
class MinHeapNode:
    state: State
    distance: float
    action: float
    parent: State
    index: int
class MinHeap:
    def left_child_index(index):
        return (2*index)+1
    def right_child_index(index):
        return 2*(index+1)
    def parent_ind(index):
        return (index-1)//2
    def __init__(self):
        self.heap: list[State] = []
        self.map: dict[State, MinHeapNode] = {}
    def bubble_down_root(self):
        curr_node = self.heap[0]
        curr_index = 0
        left_child_ind = 1
        right_child_ind = 2
        while (left_child_ind < len(self.heap)):
            smaller_child_ind = left_child_ind
            if (right_child_ind < len(self.heap) and self.map[self.heap[right_child_ind]].distance < self.map[self.heap[left_child_ind]].distance):
                smaller_child_ind = right_child_ind
            if (self.map[self.heap[smaller_child_ind]].distance >= self.map[curr_node].distance):
                break
            self.map[self.heap[smaller_child_ind]].index = curr_index
            self.heap[curr_index] = self.heap[smaller_child_ind]
            curr_index = smaller_child_ind
            left_child_ind = MinHeap.left_child_index(curr_index)
            right_child_ind = MinHeap.right_child_index(curr_index)
        self.heap[curr_index] = curr_node
        self.map[curr_node].index = curr_index
    def heap_pop(self) -> State:
        if (len(self.heap) == 0): return None
        minnode = self.heap[0]
        self.heap[0] = self.heap[-1]
        self.map[self.heap[0]].index = 0
        self.heap.pop()
        if (self.heap): self.bubble_down_root()
        return minnode
    def bubble_up(self, curr_index):
        this_node = self.heap[curr_index]
        this_distance = self.map[this_node].distance
        while (curr_index > 0):
            parent_ind = MinHeap.parent_ind(curr_index)
            if (self.map[self.heap[parent_ind]].distance <= this_distance): break
            self.map[self.heap[parent_ind]].index = curr_index
            self.heap[curr_index] = self.heap[parent_ind]
            curr_index = parent_ind
        self.map[this_node].index = curr_index
        self.heap[curr_index] = this_node
    def update_key(self, state, distance, action, parent):
        #print(f"({parent} -> {state}): (dist:{distance}, action:{action})")
        if state not in self.map:
            self.map[state] = MinHeapNode(state, distance, action, parent, len(self.heap))
            self.heap.append(state)
            #print("\t state was new, bubbling up now")
            self.bubble_up(self.map[state].index)
        elif self.map[state].distance > distance or (self.map[state].distance == distance and self.map[state].action > action):
            if (self.map[state].index == 0 and self.heap[0] != state): print("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
            self.map[state].distance = distance
            self.map[state].action = action
            self.map[state].parent = parent
            #print("\t state was not new, bubbling up now")
            self.bubble_up(self.map[state].index)

def Dijkstra_minheap(source, graph_dict):
    minheap = MinHeap()
    minheap.update_key(source, 0, 0, None)
    while ((current_node := minheap.heap_pop()) is not None):
        for distance, action, neighbor in graph_dict[current_node]:
            minheap.update_key(
                neighbor, 
                minheap.map[current_node].distance + distance, 
                minheap.map[current_node].action + action, 
                current_node
            )
    return minheap.map
def trace_path(state1: State, minheapmap: dict[State, State], state2):
    e = []
    parent = minheapmap[state2].parent
    while parent != state1: 
        e.append(parent)
        parent = minheapmap[parent].parent
    return [state1] + e[::-1] + [state2]



def Dijkstra_to_all_MOVE_STATES(startStates: State, paths):
    count = 0
    for startState in startStates:
        graph = None
        for Connected_graph in SEPARATED_TOTAL_STATES:
            if startState in Connected_graph:
                graph = Connected_graph
                break
        minheapmap = Dijkstra_minheap(startState, graph)
        count += 1
        print(f"\rDijkstra finished! {count}/{len(ROBOT_MOVE_STATES)}", end='', flush=True)
        for persp, Rstate2 in product(ALLPERSPS, ROBOT_MOVE_STATES):
            endState = State(persp, Rstate2)
            if endState == startState: continue
            if endState not in graph: continue
            paths.append(tuple(trace_path(startState, minheapmap, endState)))
    print()


PATHS = []
Dijkstra_to_all_MOVE_STATES([State(Orientation('F', 0), Rstate1) for Rstate1 in ROBOT_MOVE_STATES], PATHS)
ROOT_PATHS = []
Dijkstra_to_all_MOVE_STATES([Robot_start_state], ROOT_PATHS)
OTHER_ROOT_PATHS = []
Dijkstra_to_all_MOVE_STATES([State(Orientation('F', 0), RobotState(ArmState(1, 0), ArmState(1, 0), ArmState(1, 0), ArmState(1, 0)))], OTHER_ROOT_PATHS)

print(len(PATHS))
print(len(ROOT_PATHS))
print(len(OTHER_ROOT_PATHS))

with open("ServoOptimizationTable.txt", "w") as file:
    for path in PATHS:
        line = ' '.join([str(i.asNum()) for i in path])
        file.write(f"{line}\n")
with open("ServoOptimizationTable_rootpaths.txt", "w") as file:
    for path in ROOT_PATHS:
        line = ' '.join([str(i.asNum()) for i in path])
        file.write(f"{line}\n")
with open("ServoOptimizationTable_other_rootpaths.txt", "w") as file:
    for path in OTHER_ROOT_PATHS:
        line = ' '.join([str(i.asNum()) for i in path])
        file.write(f"{line}\n")