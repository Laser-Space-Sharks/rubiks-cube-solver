from bisect import bisect_left
from Robot_values import *
from DataTypes import *
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

def state_can_do_move(move: Move, state: State):
    face = Orientation_to_arr6[state.persp][faces_to_nums[move.face]]
    if (face == 'F' or face == 'B'): return False
    if face == 'U':
        if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
        if state.servos.R.rot == 1 or state.servos.L.rot == 1: return None
        if state.servos.U.rot == 3-move.turns: return None
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
def state_can_do_opposite_move_pair(move1: Move, move2: Move, state: State):
    face1 = Orientation_to_arr6[state.persp][faces_to_nums[move1.face]]
    if (face1 == 'F' or face1 == 'B'): return False
    if not (state.servos.U.e == state.servos.R.e == state.servos.D.e == state.servos.L.e == 1): return False
    if face1 == 'U': return (state.servos.R.rot != 1 and state.servos.L.rot != 1 and state.servos.U.rot != 3-move1.turns and state.servos.D.rot != 3-move2.turns)
    if face1 == 'R': return (state.servos.U.rot != 1 and state.servos.D.rot != 1 and state.servos.R.rot != 3-move1.turns and state.servos.L.rot != 3-move2.turns)
    if face1 == 'L': return (state.servos.U.rot != 1 and state.servos.D.rot != 1 and state.servos.L.rot != 3-move1.turns and state.servos.R.rot != 3-move2.turns)
    if face1 == 'D': return (state.servos.R.rot != 1 and state.servos.L.rot != 1 and state.servos.D.rot != 3-move1.turns and state.servos.U.rot != 3-move2.turns)

def state_after_opposite_moves_pair(move1: Move, move2: Move, state: State):
    if move1.face == 'D': return state_after_move(move2, state_after_move(move1, state)[0])
    if move2.face == 'D': return state_after_move(move1, state_after_move(move2, state)[0])
    return state_after_move(move2, state_after_move(move1, state)[0])

def algstr_to_alg(algstr: str):
    return [
        (mv[0], {"'": 3, "2": 2}.get(mv[-1], 1))
        for mv in algstr.strip().split()
    ]

def is_valid_step(state: State, state2: State):
    if state == state2: return False
    U, R, D, L = state.unpackServos()
    U2, R2, D2, L2 = state2.unpackServos()
    for v1, v2 in zip(state.unpackServos(), state2.unpackServos()):
        if v1.rot != v2.rot and (v1.e != v2.e): return False

    Etoggling = [(U.e!=U2.e), (R.e!=R2.e), (D.e!=D2.e), (L.e!=L2.e)]
    if Etoggling[0] and Etoggling[1] and U.rot == R.rot == 1: return False
    if Etoggling[1] and Etoggling[2] and R.rot == D.rot == 1: return False
    if Etoggling[2] and Etoggling[3] and D.rot == L.rot == 1: return False
    if Etoggling[3] and Etoggling[0] and L.rot == U.rot == 1: return False
    if Etoggling[2] and (Etoggling[1] or Etoggling[3]): return False
    return True

def Optimize_for_alg(algstr: str):
    print(algstr)
    alg = algstr_to_alg(algstr)
    alg_sections = []
    for mv in alg:
        if alg_sections and len(alg_sections[-1]) == 1 and alg_sections[-1][0].face == opposites[mv[0]]:
            alg_sections[-1].append(Move(mv[0], mv[1]))
        else: alg_sections.append([Move(mv[0], mv[1])])
    print(alg_sections)
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
    #########################################################################################################
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


Algorithm = "F D' R2 D' L' F L B' U R D' R F' U2 F D R U' F' D2 L U' R2 B' U2"
Optimize_for_alg(sys.argv[1])
