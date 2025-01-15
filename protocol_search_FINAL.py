from bisect import bisect_left
import time
import numpy as np
from itertools import product
import sys
sys.setrecursionlimit(100000)
#import numpy as np
TIME = time.perf_counter()

'''
#define halfTurn 500 //in Milliseconds
#define quarterTurn 350 //in Milliseconds
#define engageTime 375 //in Milliseconds
'''
Etime = 0.375
dEtime = 0.375
rot1time = 0.35
rot2time = 0.5
rot3time = 0.65
rot4time = 0.8

Y1time = rot1time
Y2time = rot2time
Y3time = rot1time
X1time = rot1time
X3time = rot1time

Umax = 180
Rmax = 180
Dmax = 180
Lmax = 180

Umin = 0
Rmin = 0
Dmin = 0
Lmin = 0

U_angles = [i for i in range(Umin, Umax+1, 90)]
R_angles = [i for i in range(Rmin, Rmax+1, 90)]
D_angles = [i for i in range(Dmin, Dmax+1, 90)]
L_angles = [i for i in range(Lmin, Lmax+1, 90)]
#######################################################################################################
######                                                                                           ######
######                                   CREATE TOTAL WEIGHTED GRAPH                             ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
ROT_TRAINS = {'F': (('U', 0), ('R', 0), ('D', 2), ('L', 0)),
            'U': (('B', 2), ('R', 1), ('F', 0), ('L', 3)),
            'R': (('U', 3), ('B', 0), ('D', 3), ('F', 0)),
            'D': (('B', 0), ('L', 3), ('F', 2), ('R', 1)),
            'L': (('U', 1), ('F', 0), ('D', 1), ('B', 0)),
            'B': (('U', 2), ('L', 0), ('D', 0), ('R', 0))}
#########################################################
##                      FUNCTIONS                      ##
#########################################################
def is_valid_state(state):
    R, L, U, D = state
    if R == (90, 1) and D == (90, 1): return False
    if L == (90, 1) and D == (90, 1): return False
    if R == (90, 1) and U == (90, 1): return False
    if L == (90, 1) and U == (90, 1): return False
    return (D[1] == 1 or (R == (90, 1) and L[1] == 1) or (L == (90, 1) and R[1] == 1))
def valid_step(state, state2):
    if state == state2: return False
    _, R, L, U, D = state
    _, R2, L2, U2, D2 = state2
    for v1, v2 in zip(state[1:], state2[1:]):
        if v1[0] != v2[0] and (v1[1] or v2[1]): return False
    Etoggling = [(U[1]!=U2[1]), (R[1]!=R2[1]), (D[1]!=D2[1]), (L[1]!=L2[1])]
    if Etoggling[0] and Etoggling[1] and (U[0]//90)%2 == (R[0]//90)%2 == 1: return False
    if Etoggling[1] and Etoggling[2] and (R[0]//90)%2 == (D[0]//90)%2 == 1: return False
    if Etoggling[2] and Etoggling[3] and (D[0]//90)%2 == (L[0]//90)%2 == 1: return False
    if Etoggling[3] and Etoggling[0] and (L[0]//90)%2 == (U[0]//90)%2 == 1: return False
    if Etoggling[2] and (Etoggling[1] or Etoggling[3]): return False
    return True
def persp_shift_y(persp0: tuple[str, int]):
    AdjFacesToNums = {'U': 0, 'R': 1, 'D': 2, 'L': 3}
    thing = ROT_TRAINS[persp0[0]][(AdjFacesToNums['R']+persp0[1])%4]
    return (thing[0], (persp0[1] + thing[1])%4)
def persp_shift_y3(persp0: tuple[str, int]):
    AdjFacesToNums = {'U': 0, 'R': 1, 'D': 2, 'L': 3}
    thing = ROT_TRAINS[persp0[0]][(AdjFacesToNums['L']+persp0[1])%4]
    return (thing[0], (persp0[1] + thing[1])%4)
def persp_shift_y2(persp0: tuple[str, int]):
    return persp_shift_y(persp_shift_y(persp0))
def persp_shift_x(persp0: tuple[str, int]):
    AdjFacesToNums = {'U': 0, 'R': 1, 'D': 2, 'L': 3}
    thing = ROT_TRAINS[persp0[0]][(AdjFacesToNums['D']+persp0[1])%4]
    return (thing[0], (persp0[1] + thing[1])%4)
def persp_shift_x3(persp0: tuple[str, int]):
    AdjFacesToNums = {'U': 0, 'R': 1, 'D': 2, 'L': 3}
    thing = ROT_TRAINS[persp0[0]][(AdjFacesToNums['U']+persp0[1])%4]
    return (thing[0], (persp0[1] + thing[1])%4)
def y1(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (0, 0, 1, 1) and U[0] <= Umax-90 and D[0] >= Dmin+90:
        for R2, L2 in product(R_angles, L_angles):
            a.add((persp_shift_y(persp), (R2, 0), (L2, 0), (U[0]+90, 1), (D[0]-90, 1)))
    return a
def y3(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (0, 0, 1, 1) and U[0] >= Umin+90 and D[0] <= Dmax-90:
        for R2, L2 in product(R_angles, L_angles):
            a.add((persp_shift_y3(persp), (R2, 0), (L2, 0), (U[0]-90, 1), (D[0]+90, 1)))
    return a
def y2(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) != (0, 0, 1, 1): return a
    if U[0] <= Umax-180 and D[0] >= Dmin+180:
        for R2, L2 in product(R_angles, L_angles):
            a.add((persp_shift_y2(persp), (R2, 0), (L2, 0), (U[0]+180, 1), (D[0]-180, 1)))
    if U[0] >= Umin+180 and D[0] <= Dmax-180:
        for R2, L2 in product(R_angles, L_angles):
            a.add((persp_shift_y2(persp), (R2, 0), (L2, 0), (U[0]-180, 1), (D[0]+180, 1)))
    return a
def x1(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) != (1, 1, 0, 0): return a
    if ((R[0]//90)%2 == (L[0]//90)%2): return a
    if R[0] <= Rmax-90 and L[0] >= Lmin+90:
        for U2, D2 in product(U_angles, D_angles):
            a.add((persp_shift_x(persp), (R[0]+90, 1), (L[0]-90, 1), (U2, 0), (D2, 0)))
    return a
def x3(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) != (1, 1, 0, 0): return a
    if ((R[0]//90)%2 == (L[0]//90)%2): return a
    if R[0] >= Rmin+90 and L[0] <= Lmax-90:
        for U2, D2 in product(U_angles, D_angles):
            a.add((persp_shift_x3(persp), (R[0]-90, 1), (L[0]+90, 1), (U2, 0), (D2, 0)))
    return a
def calc_weight(state, state2):
    MAX = 0
    for v1, v2 in zip(state[1:], state2[1:]):
        if (v1[1], v2[1]) == (0, 1): MAX = max(MAX, Etime)
        elif (v1[1], v2[1]) == (1, 0): MAX = max(MAX, dEtime)
        elif abs(v1[0] - v2[0]) == 90: MAX = max(MAX, rot1time)
        elif abs(v1[0] - v2[0]) == 180: MAX = max(MAX, rot2time)
        elif abs(v1[0] - v2[0]) == 270: MAX = max(MAX, rot3time)
        elif abs(v1[0] - v2[0]) == 360: MAX = max(MAX, rot4time)
    return MAX
#########################################################
##                         GO                          ##
#########################################################
possible_things = {}
persps = list(product("URFLBD", range(4)))

for state in product(persps, list(product(R_angles, (0, 1))), list(product(L_angles, (0, 1))), list(product(U_angles, (0, 1))), list(product(D_angles, (0, 1)))):
    if is_valid_state(state[1:]):
        possible_things[state] = []

# y1
for state in product(persps, [(i, 0) for i in R_angles],
                             [(i, 0) for i in L_angles],
                             [(i, 1) for i in U_angles[:-1]],
                             [(i, 1) for i in D_angles[1:]]):
    possible_things[state].extend([(max(calc_weight(state, i), Y1time), i) for i in y1(state)])
# y2
for state in product(persps, [(i, 0) for i in R_angles], 
                             [(i, 0) for i in L_angles],
                             [(i, 1) for i in U_angles[:-2]], 
                             [(i, 1) for i in D_angles[2:]]):
    possible_things[state].extend([(max(calc_weight(state, i), Y2time), i) for i in y2(state)])
for state in product(persps, [(i, 0) for i in R_angles], 
                             [(i, 0) for i in L_angles],
                             [(i, 1) for i in U_angles[2:]], 
                             [(i, 1) for i in D_angles[:-2]]):
    possible_things[state].extend([(max(calc_weight(state, i), Y2time), i) for i in y2(state)])
# y3
for state in product(persps, [(i, 0) for i in R_angles],
                             [(i, 0) for i in L_angles],
                             [(i, 1) for i in U_angles[1:]],
                             [(i, 1) for i in D_angles[:-1]]):
    possible_things[state].extend([(max(calc_weight(state, i), Y3time), i) for i in y3(state)])
# x1
for state in product(persps, [(i, 1) for i in R_angles[0:len(R_angles)-1:2]],
                             [(i, 1) for i in L_angles[1:len(L_angles):2]],
                             [(i, 0) for i in U_angles],
                             [(i, 0) for i in D_angles]):
    possible_things[state].extend([(max(calc_weight(state, i), X1time), i) for i in x1(state)])
for state in product(persps, [(i, 1) for i in R_angles[1:len(R_angles)-1:2]],
                             [(i, 1) for i in L_angles[2:len(L_angles):2]],
                             [(i, 0) for i in U_angles],
                             [(i, 0) for i in D_angles]):
    possible_things[state].extend([(max(calc_weight(state, i), X1time), i) for i in x1(state)])
# x3
for state in product(persps, [(i, 1) for i in R_angles[1:len(R_angles):2]],
                             [(i, 1) for i in L_angles[0:len(L_angles)-1:2]],
                             [(i, 0) for i in U_angles],
                             [(i, 0) for i in D_angles]):
    possible_things[state].extend([(max(calc_weight(state, i), X3time), i) for i in x3(state)])
for state in product(persps, [(i, 1) for i in R_angles[2:len(R_angles):2]],
                             [(i, 1) for i in L_angles[1:len(L_angles)-1:2]],
                             [(i, 0) for i in U_angles],
                             [(i, 0) for i in D_angles]):
    possible_things[state].extend([(max(calc_weight(state, i), X3time), i) for i in x3(state)])

'''
Before, We created empty nodes to address every possible servo state.
We then connected all the nodes we could with all the cube rotations we could.
Now, we will finish by focusing on no cube rotations, and permutating the possible and valid individual arm movements.
'''
for state in possible_things.keys():
    _, R, L, U, D = state
    R2states = [(R[0], 0), R] if R[1] else [(R[0], 1)] + [(i, 0) for i in R_angles]
    L2states = [(L[0], 0), L] if L[1] else [(L[0], 1)] + [(i, 0) for i in L_angles]
    U2states = [(U[0], 0), U] if U[1] else [(U[0], 1)] + [(i, 0) for i in U_angles]
    D2states = [(D[0], 0), D] if D[1] else [(D[0], 1)] + [(i, 0) for i in D_angles]
    for state2 in product([state[0]], R2states, L2states, U2states, D2states):
        if is_valid_state(state2[1:]) and valid_step(state, state2):
            possible_things[state].append((calc_weight(state, state2), state2))
print(f"# of possible servo states: {len(list(possible_things.keys()))}")
print(f"# of edges in graph: {sum(len(i) for i in possible_things.values())/2}")
print(f"-------------------------------------")

#######################################################################################################
######                                                                                           ######
######                                         ANALYZE GRAPH                                     ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
#########################################################
##                      FUNCTIONS                      ##
#########################################################
def check_connection(node, already_checked: set):
    for _, node2 in possible_things[node]:
        if node2 not in already_checked:
            already_checked.add(node2)
            check_connection(node2, already_checked)
def is_picture_state(state: tuple[tuple, tuple, tuple, tuple, tuple]):
    for i1, i2 in state[1:]:
        if i1 != 90 and i2 == 1:
            return False
    return True
#########################################################
##                         GO                          ##
#########################################################
# We will now separate the total graph into its separate connected components, if there are any.
CONNECTED_COMPONENTS = []
ServoSpace1 = set()
check_connection(list(possible_things.keys())[0], ServoSpace1)
CONNECTED_COMPONENTS.append(ServoSpace1)
for i in possible_things:
    good = False
    for space in CONNECTED_COMPONENTS:
        if i in space:
            good = True
            break
    if not good:
        newspace = set()
        check_connection(i, newspace)
        CONNECTED_COMPONENTS.append(newspace)

POSSIBLE_THINGS = [{key: possible_things[key] for key in Real_States} for Real_States in CONNECTED_COMPONENTS]

print(f"There are {len(CONNECTED_COMPONENTS)} separate connected components in the graph")
for i, v in enumerate(CONNECTED_COMPONENTS):
    print(f"nodes in servospace{i+1}: {len(v)}")
print(f"-------------------------------------")

for statesnum, Real_possible_things in enumerate(POSSIBLE_THINGS, start=1):
    print(f"WITHIN SERVOSPACE{statesnum}: ")
    count2 = 0
    for key, value in Real_possible_things.items():
        for weight, node in value:
            if (weight, key) not in Real_possible_things[node]:
                print(f"\tOH NO, {key} -> {node} IS DIRECTED!")
                count2 += 1
    print(f"\t{count2} edges are directed")
print(f"-------------------------------------")

# We will now categorize the picture states based on face.
PICTURE_STATESS = [{'U': [], 'R': [], 'F': [], 'L': [], 'B': [], 'D': []} for _ in CONNECTED_COMPONENTS]
for i1, v1 in enumerate(CONNECTED_COMPONENTS):
    for i2 in v1:
        if is_picture_state(i2):
            PICTURE_STATESS[i1][i2[0][0]].append(i2)
    e = {i3: len(v2) for i3, v2 in PICTURE_STATESS[i1].items()}
    print(f"picture states for each face in servospace{i1+1}: {e}")

# We will now determine which connected components are valid for protocol searching. 
# An invalid protocol has no picture states in any one of the categories.
VALID_GROUPS = []
for statesnum, (Picture_States, States, Real_possible_things) in enumerate(zip(PICTURE_STATESS, CONNECTED_COMPONENTS, POSSIBLE_THINGS), start=1):
    bad = False
    for i in Picture_States.values():
        if len(i) == 0:
            bad = True
    if not bad:
        print(f"servospace{statesnum} deemed valid")
        VALID_GROUPS.append((statesnum, States, Real_possible_things, Picture_States))
print(f"-------------------------------------")

#######################################################################################################
######                                                                                           ######
######                     FIND SHORTEST PATHS BETWEEN PICTURE STATES                            ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
#########################################################
##                      FUNCTIONS                      ##
#########################################################
def Dijkstra(source, graph_dict):
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
#########################################################
##                         GO                          ##
#########################################################
def trace_path(state1, parents, state2):
    e = []
    parent = parents[state2]
    while parent != state1: 
        e.append(parent)
        parent = parents[parent]
    return [state1] + e[::-1] + [state2]
def get_move_between_states(state1, state2):
    persp1, R1, L1, U1, D1 = state1
    persp2, R2, L2, U2, D2 = state2
    move = []
    if persp1 != persp2:
        if persp_shift_y(persp1) == persp2:
            move.append("y")
            if R2[0] == R1[0]+90: move.append("R")
            if R2[0] == R1[0]+180: move.append("R2")
            if R2[0] == R1[0]-180: move.append("R2'")
            if R2[0] == R1[0]-90: move.append("R'")
            if L2[0] == L1[0]+90: move.append("L")
            if L2[0] == L1[0]+180: move.append("L2")
            if L2[0] == L1[0]-180: move.append("L2'")
            if L2[0] == L1[0]-90: move.append("L'")
        elif persp_shift_y2(persp1) == persp2:
            move.append("y2")
            if R2[0] == R1[0]+90: move.append("R")
            if R2[0] == R1[0]+180: move.append("R2")
            if R2[0] == R1[0]-180: move.append("R2'")
            if R2[0] == R1[0]-90: move.append("R'")
            if L2[0] == L1[0]+90: move.append("L")
            if L2[0] == L1[0]+180: move.append("L2")
            if L2[0] == L1[0]-180: move.append("L2'")
            if L2[0] == L1[0]-90: move.append("L'")
        elif persp_shift_y3(persp1) == persp2:
            move.append("y'")
            if R2[0] == R1[0]+90: move.append("R")
            if R2[0] == R1[0]+180: move.append("R2")
            if R2[0] == R1[0]-180: move.append("R2'")
            if R2[0] == R1[0]-90: move.append("R'")
            if L2[0] == L1[0]+90: move.append("L")
            if L2[0] == L1[0]+180: move.append("L2")
            if L2[0] == L1[0]-180: move.append("L2'")
            if L2[0] == L1[0]-90: move.append("L'")
        elif persp_shift_x(persp1) == persp2:
            move.append("x")
            if U2[0] == U1[0]+90: move.append("U")
            if U2[0] == U1[0]+180: move.append("U2")
            if U2[0] == U1[0]-180: move.append("U2'")
            if U2[0] == U1[0]-90: move.append("U'")
            if D2[0] == D1[0]+90: move.append("D")
            if D2[0] == D1[0]+180: move.append("D2")
            if D2[0] == D1[0]-180: move.append("D2'")
            if D2[0] == D1[0]-90: move.append("D'")
        elif persp_shift_x3(persp1) == persp2:
            move.append("x'")
            if U2[0] == U1[0]+90: move.append("U")
            if U2[0] == U1[0]+180: move.append("U2")
            if U2[0] == U1[0]-180: move.append("U2'")
            if U2[0] == U1[0]-90: move.append("U'")
            if D2[0] == D1[0]+90: move.append("D")
            if D2[0] == D1[0]+180: move.append("D2")
            if D2[0] == D1[0]-180: move.append("D2'")
            if D2[0] == D1[0]-90: move.append("D'")
    else:
        if R2[0] == R1[0]+90: move.append("R")
        if R2[0] == R1[0]+180: move.append("R2")
        if R2[0] == R1[0]-180: move.append("R2'")
        if R2[0] == R1[0]-90: move.append("R'")
        if R2[1] == R1[1]+1: move.append("Re")
        if R2[1] == R1[1]-1: move.append("Re'")
        if L2[0] == L1[0]+90: move.append("L")
        if L2[0] == L1[0]+180: move.append("L2")
        if L2[0] == L1[0]-180: move.append("L2'")
        if L2[0] == L1[0]-90: move.append("L'")
        if L2[1] == L1[1]+1: move.append("Le")
        if L2[1] == L1[1]-1: move.append("Le'")
        if U2[0] == U1[0]+90: move.append("U")
        if U2[0] == U1[0]+180: move.append("U2")
        if U2[0] == U1[0]-180: move.append("U2'")
        if U2[0] == U1[0]-90: move.append("U'")
        if U2[1] == U1[1]+1: move.append("Ue")
        if U2[1] == U1[1]-1: move.append("Ue'")
        if D2[0] == D1[0]+90: move.append("D")
        if D2[0] == D1[0]+180: move.append("D2")
        if D2[0] == D1[0]-180: move.append("D2'")
        if D2[0] == D1[0]-90: move.append("D'")
        if D2[1] == D1[1]+1: move.append("De")
        if D2[1] == D1[1]-1: move.append("De'")
    return '.'.join(move)
def move_sequence_between_picturestates(state1, parents, state2):
    e = trace_path(state1, parents, state2)
    e2 = []
    for i in range(len(e)-1):
        e2.append(get_move_between_states(e[i], e[i+1]))
    return ' '.join(e2)



    
PICTURE_GRAPHS = []
print("Using Dijkstra to connect Picture states...")
for statesnum, Real_States, Real_possible_things, Real_Picture_States in VALID_GROUPS:
    print(f"WITHIN SERVOSPACE{statesnum}: ")
    Picture_Graph = {}
    others = ["RFLBD", "UFLBD", "URLBD", "URFBD", "URFLD", "URFLB"]
    count = 0
    for i, face in enumerate("URFLBD"):
        for Real_Picture_State in Real_Picture_States[face]:
            distances, parents = Dijkstra(Real_Picture_State, Real_possible_things)
            if np.inf in distances.values(): print('AAAAAAAAAAAAAAAAAAAAAAAAA')
            count += 1
            print(f"\r\tDijkstra finished! {count}/{sum(len(i) for i in Real_Picture_States.values())}", end='', flush=True)
            #print(len(distances))
            Picture_Graph[Real_Picture_State] = {}
            for face2 in others[i]:
                for Real_Picture_State2 in Real_Picture_States[face2]:
                    Picture_Graph[Real_Picture_State][Real_Picture_State2] = (distances[Real_Picture_State2], move_sequence_between_picturestates(Real_Picture_State, parents, Real_Picture_State2))
    PICTURE_GRAPHS.append(Picture_Graph)
    print()
print(f"-------------------------------------")
#######################################################################################################
######                                                                                           ######
######                                     FORM PROTOCOLS                                        ######
######                                                                                           ######
######                                                                                           ######
#######################################################################################################
#########################################################
##                      FUNCTIONS                      ##
#########################################################
def Step(Path_so_far, Picture_Graph, Picture_States, Options, best: list, cost_so_far):
    if not Options: 
        if cost_so_far <= best[0]:
            if cost_so_far < best[0]:
                #print(f'\t--------- protocol duration: {cost_so_far}s ------------')
                #for i in Path_so_far:
                #    print(f"\t{i}")
                best[1].clear()
                best[0] = cost_so_far
            new_Path_so_far = []
            for i in range(len(Path_so_far)-1):
                new_Path_so_far.append((Path_so_far[i], Picture_Graph[Path_so_far[i]][Path_so_far[i+1]][1]))
            new_Path_so_far.append((Path_so_far[-1], ""))
            best[1].append(new_Path_so_far)
        return
    for option in Options:
        for node in Picture_States[option]:
            cost = Picture_Graph[Path_so_far[-1]][node][0]
            if cost_so_far + cost <= best[0]:
                Step(Path_so_far + [node], Picture_Graph, Picture_States, Options - {option}, best, cost_so_far + cost)
    #for node, (cost, alg) in Picture_Graph[Path_so_far[-1]].items():
    #    if node[0][0] in Options and cost_so_far + cost <= best[0]:
    #        Step(Path_so_far + [node], Picture_Graph, Picture_States, best, cost_so_far + cost)
#########################################################
##                         GO                          ##
#########################################################


BEST_PROTOCOLS = [100, []]
print(f"Searching possible UF protocols...")
for (statesnum, Real_States, Real_possible_things, Real_Picture_States), Picture_Graph in zip(VALID_GROUPS, PICTURE_GRAPHS):
    print(f"WITHIN SERVOSPACE{statesnum}: ")
    count5 = 0
    for node in Real_Picture_States['U']:
        for node2 in Real_Picture_States['F']:
            Step([node, node2], Picture_Graph, Real_Picture_States, {"U", "R", "F", "L", "B", "D"}-{"U", "F"}, BEST_PROTOCOLS, Picture_Graph[node][node2][0])
        count5 += 1
        print(f"\r\tFirst Node affected!: {count5}/{len(Real_Picture_States['U'])}", end='', flush=True)
    print()

print(f"\n# of tied protocols at best weight {BEST_PROTOCOLS[0]}: {len(BEST_PROTOCOLS[1])}")

with open("protocols.txt", "w") as file:
    for protocol in BEST_PROTOCOLS[1]:
        file.write('------------------------------------\n')
        for node, alg in protocol:
            file.write(f"{node} | {alg}\n")


print(time.perf_counter() - TIME)