from bisect import bisect_left
from dataclasses import dataclass
from itertools import product
import numpy as np
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
faces_to_nums = {'U': 0, 'R': 1, 'F': 2, 'L': 3, 'B': 4, 'D': 5}

@dataclass(frozen=True, eq=True)
class Orientation:
    face: str
    rot: int
    #def __init__(self, face: str, rotation: int):
    #    self.face: str = face
    #    self.rot: int = rotation
    def asTuple(self) -> tuple[str, int]:
        return (self.face, self.rot)
    #def __eq__(self, other: 'Orientation'):
    #    if (not isinstance(other, Orientation)): return False
    #    return (other.face == self.face and other.rot == self.rot)
    #def __hash__(self):
    #    return hash(self.asTuple())
    def __repr__(self):
        return f"Orientation('{self.face}', {self.rot})"
    def asNum(self):
        return (faces_to_nums[self.face]*4 + self.rot)
    
Orientation_to_arr6 = {
    Orientation('U', 0): "FRDLUB",
    Orientation('U', 1): "FURDLB",
    Orientation('U', 2): "FLURDB",
    Orientation('U', 3): "FDLURB",

    Orientation('R', 0): "UFLBRD",
    Orientation('R', 1): "LFDBUR",
    Orientation('R', 2): "DFRBLU",
    Orientation('R', 3): "RFUBDL",

    Orientation('F', 0): "URFLBD",
    Orientation('F', 1): "LUFDBR",
    Orientation('F', 2): "DLFRBU",
    Orientation('F', 3): "RDFUBL",

    Orientation('L', 0): "UBRFLD",
    Orientation('L', 1): "LBUFDR",
    Orientation('L', 2): "DBLFRU",
    Orientation('L', 3): "RBDFUL",

    Orientation('B', 0): "ULBRFD",
    Orientation('B', 1): "LDBUFR",
    Orientation('B', 2): "DRBLFU",
    Orientation('B', 3): "RUBDFL",

    Orientation('D', 0): "BRULDF",
    Orientation('D', 1): "BULDRF",
    Orientation('D', 2): "BLDRUF",
    Orientation('D', 3): "BDRULF",
}
arr6_to_Orientation = {
    "FRDLUB": Orientation('U', 0),
    "FURDLB": Orientation('U', 1),
    "FLURDB": Orientation('U', 2),
    "FDLURB": Orientation('U', 3),

    "UFLBRD": Orientation('R', 0),
    "LFDBUR": Orientation('R', 1),
    "DFRBLU": Orientation('R', 2),
    "RFUBDL": Orientation('R', 3),

    "URFLBD": Orientation('F', 0),
    "LUFDBR": Orientation('F', 1),
    "DLFRBU": Orientation('F', 2),
    "RDFUBL": Orientation('F', 3),

    "UBRFLD": Orientation('L', 0),
    "LBUFDR": Orientation('L', 1),
    "DBLFRU": Orientation('L', 2),
    "RBDFUL": Orientation('L', 3),

    "ULBRFD": Orientation('B', 0),
    "LDBUFR": Orientation('B', 1),
    "DRBLFU": Orientation('B', 2),
    "RUBDFL": Orientation('B', 3),

    "BRULDF": Orientation('D', 0),
    "BULDRF": Orientation('D', 1),
    "BLDRUF": Orientation('D', 2),
    "BDRULF": Orientation('D', 3),
}
def Multiply_arr6s(arr6_1, arr6_2):
    return [arr6_2[faces_to_nums[i]] for i in arr6_1]

    
@dataclass(frozen=True, eq=True)
class RoboMove:
    face: str
    turns: int
    #def __init__(self, face: str, turns: int):
    #    self.face: str = face
    #    self.turns: int = turns
    def asStr(self) -> str:
        if self.turns == 1: return f"{self.face}"
        if self.turns == 2: return f"{self.face}2"
        if self.turns == 3: return f"{self.face}'"
    #def __eq__(self, other: 'RoboMove'):
    #    if (not isinstance(other, RoboMove)): return False
    #    return (self.face == other.face and self.turns == other.turns)
    #def __hash__(self):
    #    return hash(self.asStr())
    def __repr__(self):
        return f"({self.face}, {self.turns})"
@dataclass(frozen=True, eq=True)
class Move:
    face: str
    turns: int
    #def __init__(self, face: str, turns: int):
    #    self.face: str = face
    #    self.turns: int = turns
    def asStr(self) -> str:
        if self.turns == 1: return f"{self.face}"
        if self.turns == 2: return f"{self.face}2"
        if self.turns == 3: return f"{self.face}'"
    #def __eq__(self, other: 'Move'):
    #    if (not isinstance(other, Move)): return False
    #    return (self.face == other.face and self.turns == other.turns)
    #def __hash__(self):
    #    return hash(self.asStr())
    def __repr__(self):
        return f"({self.face}, {self.turns})"
@dataclass(frozen=True, eq=True)
class CubeRotation:
    rotation: str
    #def __init__(self, rotation: str):
    #    self.rotation: str = rotation
    def asStr(self) -> str:
        return self.rotation
    #def __eq__(self, other: 'CubeRotation'):
    #    if (not isinstance(other, CubeRotation)): return False
    #    return (self.rotation == other.rotation)
    #def __hash__(self):
    #    return hash(self.rotation)
    def __repr__(self):
        return f"{self.rotation}"
@dataclass(frozen=True, eq=True)
class ArmState:
    e: bool
    rot: int
    #def __init__(self, e: bool, rot: int):
    #    self.e: bool = e
    #    self.rot: int = rot
    #def __eq__(self, other: 'ArmState'):
    #    if (not isinstance(other, ArmState)): return False
    #    return (self.e == other.e and self.rot == other.rot)
    #def __hash__(self):
    #    return hash((self.e, self.rot))
    def __repr__(self):
        return f"ArmState({self.e}, {self.rot})"
@dataclass(frozen=True, eq=True)
class RobotState:
    U: ArmState
    R: ArmState
    D: ArmState
    L: ArmState
    #def __init__(self, armU: ArmState, armR: ArmState, armD: ArmState, armL: ArmState):
    #    self.U: ArmState = armU
    #    self.R: ArmState = armR
    #    self.D: ArmState = armD
    #    self.L: ArmState = armL
    def as2B(self) -> int:
        bitmask = 0
        bitmask *= 2; bitmask |= self.U.e
        bitmask *= 2; bitmask |= self.R.e
        bitmask *= 2; bitmask |= self.D.e
        bitmask *= 2; bitmask |= self.L.e
        bitmask *= 3; bitmask |= self.U.rot
        bitmask *= 3; bitmask |= self.R.rot
        bitmask *= 3; bitmask |= self.D.rot
        bitmask *= 3; bitmask |= self.L.rot
        return bitmask
    #def __eq__(self, other: 'RobotState'):
    #    if (not isinstance(other, RobotState)): return False
    #    return (self.as2B == other.as2B())
    #def __hash__(self):
    #    return self.as2B()
    def __repr__(self):
        return f"RobotState({self.U}, {self.R}, {self.D}, {self.L})"
@dataclass(frozen=True, eq=True)
class State:
    persp: Orientation
    servos: RobotState
    #def __init__(self, persp: Orientation, Servos: RobotState):
    #    self.persp: Orientation = persp
    #    self.servos: RobotState = Servos
    def unpack(self) -> tuple[Orientation, ArmState, ArmState, ArmState, ArmState]:
        return (self.persp, self.servos.U, self.servos.R, self.servos.D, self.servos.L)
    def unpackServos(self):
        return (self.servos.U, self.servos.R, self.servos.D, self.servos.L)
    #def __eq__(self, other: 'State'):
    #    if (not isinstance(other, State)): return False
    #    return (self.persp == other.persp and self.servos == other.servos)
    #def __hash__(self):
    #    return hash(self.unpack())
    def __repr__(self):
        return f"State({self.persp}, {self.servos})"
    def as2B(self):
        return (self.persp.asNum()*1296 + self.servos.as2B())

        
    


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

Etime = 1
dEtime = 1
rot1time = 1
rot2time = 1

Y1time = 1
Y2time = 1
Y3time = 1
X1time = 1
X3time = 1

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
def startState_to_endState_in_TOTAL_STATES(startState, endState, states):
    for _, state in states[startState]:
        if state == endState: return True
    return False
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
        TOTAL_STATES[state].append((max(calc_weight(startState, endState), Y1time), endState))

# y2
for persp, armR, armL, (armU, armD) in product(ALLPERSPS, UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS,
                                                          [(ArmState(1, 0), ArmState(1, 2)), (ArmState(1, 2), ArmState(1, 0))]):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in y2(startState):
        TOTAL_STATES[state].append((max(calc_weight(startState, endState), Y2time), endState))

# y3
for persp, armU, armR, armD, armL in product(ALLPERSPS, [ArmState(1, 1), ArmState(1, 2)],
                                                        UNEXTENDED_ARMS,
                                                        [ArmState(1, 0), ArmState(1, 1)],
                                                        UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in y3(startState):
        TOTAL_STATES[state].append((max(calc_weight(startState, endState), Y3time), endState))

# x1
for persp, (armR, armL), armU, armD in product(ALLPERSPS, [(ArmState(1, 1), ArmState(1, 2)), (ArmState(1, 0), ArmState(1, 1))],
                                                          UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in x1(startState):
        TOTAL_STATES[state].append((max(calc_weight(startState, endState), X1time), endState))

# x3
for persp, (armR, armL), armU, armD in product(ALLPERSPS, [(ArmState(1, 1), ArmState(1, 0)), (ArmState(1, 2), ArmState(1, 1))],
                                                          UNEXTENDED_ARMS,
                                                          UNEXTENDED_ARMS):
    startState = State(persp, RobotState(armU, armR, armD, armL))
    for endState in x3(startState):
        TOTAL_STATES[state].append((max(calc_weight(startState, endState), X3time), endState))

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

PATHS = []
count = 0
for Rstate1 in ROBOT_MOVE_STATES:
    _, parents = Dijkstra(State(Orientation('F', 0), Rstate1), TOTAL_STATES)
    count += 1
    print(f"\r\tDijkstra finished! {count}/{len(ROBOT_MOVE_STATES)}", end='', flush=True)
    for persp, Rstate2 in product(ALLPERSPS, ROBOT_MOVE_STATES):
        if persp == Orientation('F', 0) and Rstate1 == Rstate2: continue
        if State(persp, Rstate2) not in parents: continue
        PATHS.append(trace_path(State(Orientation('F', 0), Rstate1), parents, State(persp, Rstate2)))
print()
print(len(PATHS))

with open("ServoOptimizationTable.txt", "w") as file:
    for path in PATHS:
        line = ' '.join([str(i.as2B()) for i in path])
        file.write(f"{line}\n")
