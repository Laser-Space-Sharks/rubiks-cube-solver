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

class Orientation:
    def __init__(self, face: str, rotation: int):
        if (face not in ('U', 'R', 'F' 'L', 'B', 'D') or not (0 <= rotation < 4)): 
            raise ValueError(f"inappropriate Orientation init: ({face}, {rotation})")
        self.face = face
        self.rot = rotation
    def asTuple(self):
        return (self.face, self.rot)
class RoboMove:
    def __init__(self, face: str, turns: int):
        if (face not in ('U', 'R', 'D', 'L') or not (0 < turns < 4)): 
            raise ValueError(f"inappropriate RoboMove init: ({face}, {turns})")
        self.face = face
        self.turns = turns
    def asStr(self):
        if self.turns == 1: return f"{self.face}"
        if self.turns == 2: return f"{self.face}2"
        if self.turns == 3: return f"{self.face}'"
class Move:
    def __init__(self, face: str, turns: int):
        if (face not in ('U', 'R', 'F' 'L', 'B', 'D') or not (0 < turns < 4)): 
            raise ValueError(f"inappropriate Move init: ({face}, {turns})")
        self.face = face
        self.turns = turns
    def asStr(self):
        if self.turns == 1: return f"{self.face}"
        if self.turns == 2: return f"{self.face}2"
        if self.turns == 3: return f"{self.face}'"
class CubeRotation:
    def __init__(self, rotation: str):
        if (rotation not in ("X", "X'", "Y", "Y'", "Y2")): 
            raise ValueError(f"inappropriate CubeRotation init: {rotation}")
        self.rotation = rotation
    def asStr(self):
        return self.rotation

AdjFaces: list[str] = [
    'U', 'R', 'D', 'L'
]
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

