from dataclasses import dataclass

faces_to_nums = {'U': 0, 'R': 1, 'F': 2, 'L': 3, 'B': 4, 'D': 5}
opposites = {'U': 'D', 'R': 'L', 'F': 'B', 'L': 'R', 'B': 'F', 'D': 'U'}

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
    return ''.join([arr6_2[faces_to_nums[i]] for i in arr6_1])
    
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
    def asNum(self) -> int:
        bitmask = 0
        bitmask *= 2; bitmask += self.U.e
        bitmask *= 2; bitmask += self.R.e
        bitmask *= 2; bitmask += self.D.e
        bitmask *= 2; bitmask += self.L.e
        bitmask *= 3; bitmask += self.U.rot
        bitmask *= 3; bitmask += self.R.rot
        bitmask *= 3; bitmask += self.D.rot
        bitmask *= 3; bitmask += self.L.rot
        return bitmask
    def as2B(self):
        bitmask = 0
        bitmask <<= 1; bitmask |= self.U.e
        bitmask <<= 1; bitmask |= self.R.e
        bitmask <<= 1; bitmask |= self.D.e
        bitmask <<= 1; bitmask |= self.L.e
        bitmask <<= 2; bitmask |= self.U.rot
        bitmask <<= 2; bitmask |= self.R.rot
        bitmask <<= 2; bitmask |= self.D.rot
        bitmask <<= 2; bitmask |= self.L.rot
        return bitmask
    def asCommand(self):
        string = ""
        string += ("N" if self.U.e else "n") + "."
        string += ("E" if self.R.e else "e") + "."
        string += ("S" if self.D.e else "s") + "."
        string += ("W" if self.L.e else "w") + "."
        string += f"U{self.U.rot}.R{self.R.rot}.D{self.D.rot}.L{self.L.rot}"
        return string
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
    def asNum(self):
        return (self.persp.asNum()*1296 + self.servos.asNum())