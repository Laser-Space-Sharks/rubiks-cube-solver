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
Etime = 0.300
dEtime = 0.300
rot1time = 0.35
rot2time = 0.5

Y1time = 0.35
Y2time = 0.5
Y3time = 0.35
X1time = 0.35
X3time = 0.35

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
def is_valid_state(state):
    R, L, U, D = state
    if R == (90, 1) and D == (90, 1): return False
    if L == (90, 1) and D == (90, 1): return False
    if R == (90, 1) and U == (90, 1): return False
    if L == (90, 1) and U == (90, 1): return False
    return (D[1] == 1 or (R == (90, 1) and L[1] == 1) or (L == (90, 1) and R[1] == 1))
def y1(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (0, 0, 1, 1) and  U[0] < 180 and D[0] > 0:
        for R2, L2 in product((0, 90, 180), (0, 90, 180)):
            a.add((persp_shift_y(persp), (R2, 0), (L2, 0), (U[0]+90, 1), (D[0]-90, 1)))
    return a
def y3(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (0, 0, 1, 1) and  U[0] > 0 and D[0] < 180:
        for R2, L2 in product((0, 90, 180), (0, 90, 180)):
            a.add((persp_shift_y3(persp), (R2, 0), (L2, 0), (U[0]-90, 1), (D[0]+90, 1)))
    return a
def y2(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (0, 0, 1, 1) and abs(U[0]-D[0])==180:
        for R2, L2 in product((0, 90, 180), (0, 90, 180)):
            a.add((persp_shift_y2(persp), (R2, 0), (L2, 0), (D[0], 1), (U[0], 1)))
    return a
def x1(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (1, 1, 0, 0) and  (R[0], L[0]) in ((90, 180), (0, 90)):
        for U2, D2 in product((0, 90, 180), (0, 90, 180)):
            a.add((persp_shift_x(persp), (R[0]+90, 1), (L[0]-90, 1), (U2, 0), (D2, 0)))
    return a
def x3(state):
    persp, R, L, U, D = state
    a = set()
    if (R[1], L[1], U[1], D[1]) == (1, 1, 0, 0) and  (R[0], L[0]) in ((180, 90), (90, 0)):
        for U2, D2 in product((0, 90, 180), (0, 90, 180)):
            a.add((persp_shift_x3(persp), (R[0]-90, 1), (L[0]+90, 1), (U2, 0), (D2, 0)))
    return a
def calc_weight(state, state2):
    MAX = 0
    for v1, v2 in zip(state[1:], state2[1:]):
        if (v1[1], v2[1]) == (0, 1): MAX = max(MAX, Etime)
        elif (v1[1], v2[1]) == (1, 0): MAX = max(MAX, dEtime)
        elif abs(v1[0] - v2[0]) == 90: MAX = max(MAX, rot1time)
        elif abs(v1[0] - v2[0]) == 180: MAX = max(MAX, rot2time)
    return MAX
def calc_action(state, state2):
    SUM = 0
    for v1, v2 in zip(state[1:], state2[1:]):
        if (v1[1], v2[1]) == (0, 1): SUM += Etime
        elif (v1[1], v2[1]) == (1, 0): SUM += dEtime
        elif abs(v1[0] - v2[0]) == 90: SUM += rot1time
        elif abs(v1[0] - v2[0]) == 180: SUM += rot2time
    return SUM
def valid_step(state, state2):
    if state == state2: return False
    _, R, L, U, D = state
    _, R2, L2, U2, D2 = state2
    for v1, v2 in zip(state[1:], state2[1:]):
        if v1[0] != v2[0] and (v1[1] or v2[1]): return False
    Etoggling = [(U[1]!=U2[1]), (R[1]!=R2[1]), (D[1]!=D2[1]), (L[1]!=L2[1])]
    if Etoggling[0] and Etoggling[1] and U[0] == R[0] == 90: return False
    if Etoggling[1] and Etoggling[2] and R[0] == D[0] == 90: return False
    if Etoggling[2] and Etoggling[3] and D[0] == L[0] == 90: return False
    if Etoggling[3] and Etoggling[0] and L[0] == U[0] == 90: return False
    if Etoggling[2] and (Etoggling[1] or Etoggling[3]): return False
    return True
#########################################################
##                         GO                          ##
#########################################################
possible_things = {}
persps = list(product("URFLBD", range(4)))

servo_states = [(0, 0), (90, 0), (180, 0),
                (0, 1), (90, 1), (180, 1)]
for state in product(persps, servo_states, servo_states, servo_states, servo_states):
    if is_valid_state(state[1:]):
        possible_things[state] = []

# y1
for state in product(persps, [(0, 0), (90, 0), (180, 0)],
                             [(0, 0), (90, 0), (180, 0)],
                             [(90, 1), (0, 1)],
                             [(180, 1), (90, 1)]):
    possible_things[state].extend([(max(calc_weight(state, i), calc_action(state, i), Y1time), i) for i in y1(state)])
# y2
for persp, R, L, (U, D) in product(persps, [(0, 0), (90, 0), (180, 0)],
                                    [(0, 0), (90, 0), (180, 0)],
                                    [((0, 1), (180, 1)), ((180, 1), (0, 1))]):
    state = (persp, R, L, U, D)
    possible_things[state].extend([(max(calc_weight(state, i), calc_action(state, i), Y2time), i) for i in y2(state)])
# y3
for state in product(persps, [(0, 0), (90, 0), (180, 0)],
                             [(0, 0), (90, 0), (180, 0)],
                             [(90, 1), (180, 1)],
                             [(0, 1), (90, 1)]):
    possible_things[state].extend([(max(calc_weight(state, i), calc_action(state, i), Y3time), i) for i in y3(state)])
# x1
for persp, (R, L), U, D in product(persps, [((90, 1), (180, 1)), ((0, 1), (90, 1))],
                                           [(0, 0), (90, 0), (180, 0)],
                                           [(0, 0), (90, 0), (180, 0)]):
    state = (persp, R, L, U, D)
    possible_things[state].extend([(max(calc_weight(state, i), calc_action(state, i), X1time), i) for i in x1(state)])
# x3
for persp, (R, L), U, D in product(persps, [((90, 1), (0, 1)), ((180, 1), (90, 1))],
                                           [(0, 0), (90, 0), (180, 0)],
                                           [(0, 0), (90, 0), (180, 0)]):
    state = (persp, R, L, U, D)
    possible_things[state].extend([(max(calc_weight(state, i), calc_action(state, i), X3time), i) for i in x3(state)])

'''
Before, We created empty nodes to address every possible servo state.
We then connected all the nodes we could with all the cube rotations we could.
Now, we will finish by focusing on no cube rotations, and permutating the possible and valid individual arm movements.
'''
for state in possible_things.keys():
    states2 = []
    for i in range(1, 5):
        if state[i][1]: states2.append([(state[i][0], 0), state[i]])
        else: states2.append([(state[i][0], 1), (0, 0), (90, 0), (180, 0)])
    for a1, a2, a3, a4 in product(states2[0], states2[1], states2[2], states2[3]):
        state2 = (state[0], a1, a2, a3, a4)
        if is_valid_state(state2[1:]) and valid_step(state, state2):
            possible_things[state].append((calc_weight(state, state2), calc_action(state, state2), state2))
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
    for _, _, node2 in possible_things[node]:
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
        for weight, action, node in value:
            if (weight, action, key) not in Real_possible_things[node]:
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
class MinHeapNode:
    def __init__(self, state, weight, action, parent, index):
        self.state = state
        self.weight = weight
        self.action = action
        self.parent = parent
        self.index = index
class MinHeap:
    def left_child_index(index):
        return (2*index)+1
    def right_child_index(index):
        return 2*(index+1)
    def parent_ind(index):
        return (index-1)//2
    def __init__(self):
        self.heap = []
        self.map = {}
    '''
    MinHeapNode* MinHeap_pluck_min(MinHeap* minheap) { //printf("\t\tMinHeap_pluck_min() was called!: \n");
        if (minheap->size == 0) return NULL; 
        //printf("This plucking time... minheap size was (%zu,%zu), and min was: ", minheap->size, minheap->SupposedSize);
        //print_MinHeapNode(minheap->heap[0]);
        //printf("\t\tline 191: minheap->capacity: %zu\n", minheap->capacity);
        // save minnode
        MinHeapNode* minnode = minheap->heap[0];
        // move last into root
        MinHeapNode* curr_node = minheap->heap[minheap->size-1];
        minheap->heap[minheap->size-1] = NULL;
        minheap->size--;
        if (minheap->size == 0) return minnode;
        //printf("\t\tline 199\n");

        // bubble down
        size_t curr_index = 0;
        size_t left_child_ind = left_child_index(curr_index);
        size_t right_child_ind = right_child_index(curr_index);
        while (left_child_ind < minheap->size) {
            size_t smaller_child_ind = left_child_ind; //printf("\t\t\tline 206: right_child_ind: %zu, left_child_ind: %zu\n", right_child_ind, left_child_ind);
            //if (minheap->heap[right_child_ind] == NULL) printf("\t\t\tminheap->heap[right_child_ind] == NULL\n");
            //if (minheap->heap[left_child_ind] == NULL) printf("\t\t\tminheap->heap[left_child_ind] == NULL\n");
            if (right_child_ind < minheap->size && minheap->heap[right_child_ind]->weight < minheap->heap[left_child_ind]->weight) {
                smaller_child_ind = right_child_ind;
            } //printf("\t\t\tline 211\n");
            if (minheap->heap[smaller_child_ind]->weight < curr_node->weight) { //printf("\t\t\tline 212\n");
                minheap->heap[smaller_child_ind]->heapIndex = curr_index;
                minheap->heap[curr_index] = minheap->heap[smaller_child_ind];
                curr_index = smaller_child_ind;
                left_child_ind = left_child_index(curr_index);
                right_child_ind = right_child_index(curr_index); //printf("\t\t\tline 217\n");
            } else break;
        } //printf("\t\tline 217\n");
        minheap->heap[curr_index] = curr_node;
        curr_node->heapIndex = curr_index;
        //printf("\t\tline 220\n");
        return minnode;
    }
    '''
    def heap_pop(self) -> MinHeapNode:
        if (len(self.heap) == 0): return None
        minnode = self.heap[0]
        curr_node = self.heap[-1]
        self.heap[0] = curr_node
        self.heap.pop()
        curr_index = 0
        left_child_ind = MinHeap.left_child_index(curr_index)
        right_child_ind = MinHeap.right_child_index(curr_index)
        while (left_child_ind < len(self.heap)):
            smaller_child_ind = left_child_ind
            if (right_child_ind < len(self.heap) and self.heap[right_child_ind].weight < self.heap[left_child_ind].weight):
                smaller_child_ind = right_child_ind
            if (self.heap[smaller_child_ind].weight < curr_node.weight):
                self.heap[smaller_child_ind].index = curr_index
                self.heap[curr_index] = self.heap[smaller_child_ind]
                curr_index = smaller_child_ind
                left_child_ind = MinHeap.left_child_index(curr_index)
                right_child_ind = MinHeap.right_child_index(curr_index)
            else: break
        self.heap[curr_index] = curr_node
        curr_node.index = curr_index
        return minnode
    '''
    void MinHeap_bubble_up(MinHeap* minheap, size_t curr_index) {
        MinHeapNode* this_node = minheap->heap[curr_index];
        while (curr_index > 0) {
            size_t parent_ind = parent_index(curr_index);
            if (minheap->heap[parent_ind]->weight > this_node->weight) {
                minheap->heap[parent_ind]->heapIndex = curr_index;
                minheap->heap[curr_index] = minheap->heap[parent_ind];
                curr_index = parent_ind;
            } else break;
        }
        this_node->heapIndex = curr_index;
        minheap->heap[curr_index] = this_node;
    }
    '''
    def bubble_up(self, curr_index):
        this_node = self.heap[curr_index]
        while (curr_index > 0):
            parent_ind = MinHeap.parent_ind(curr_index)
            if (self.heap[parent_ind].weight <= this_node.weight): break
            self.heap[parent_ind].index = curr_index
            self.heap[curr_index] = self.heap[parent_ind]
        this_node.index = curr_index
        self.heap[curr_index] = this_node
    def update_key(self, )

        






def Dijkstra(source, graph_dict):
    l = len(list(graph_dict.keys()))
    distances = {key: [np.inf,np.inf] for key in graph_dict.keys()}; distances[source] = [0, 0]
    parents = {source: None}
    priority_queue_nodes = [source]
    priority_queue_nums = [0]
    for i in graph_dict.keys(): 
        if i != source: 
            priority_queue_nodes.append(i)
            priority_queue_nums.append(distances[i])
    for starting_index in range(l):
        current_node = priority_queue_nodes[starting_index]
        for weight, action, neighbor in graph_dict[current_node]:
            dist = weight + distances[current_node][0]
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
            print(f"\r\tDijkstra finished! {count}/{sum(len(j) for j in Real_Picture_States.values())}", end='', flush=True)
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