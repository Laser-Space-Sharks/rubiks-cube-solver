'''
              e        w        n        s

     O        R        L        U        D
(('U', 3), (90, 1), (90, 1), (180, 0), (0, 0)) | De Re'.Le'.Ue y'.R.L
(('F', 3), (180, 0), (180, 0), (90, 1), (90, 1)) | y'.R'.L' Re.Le Ue'.De'
(('D', 1), (90, 1), (90, 1), (0, 0), (180, 0)) | De Re' R.U Re De' x'.U.D2' De Le' L'.Ue Le Ue'.De'
(('L', 0), (90, 1), (90, 1), (180, 0), (0, 0)) | De Re'.Le'.Ue y'.R'.L'
(('B', 0), (0, 0), (0, 0), (90, 1), (90, 1)) | y'.R.L Re.Le Ue'.De'
(('R', 0), (90, 1), (90, 1), (0, 0), (180, 0)) | 


('U', 3) n.E.s.W.U2.R1.D0.L1   ************
n.E.S.W.U2.R1.D0.L1
N.e.S.w.U2.R1.D0.L1
('F', 3) N.e.S.w.U1.R2.D1.L2   ************
N.e.S.w.U0.R1.D2.L1
N.E.S.W.U0.R1.D2.L1
('D', 3) n.E.s.W.U0.R1.D2.L1   ************
n.E.S.W.U0.R1.D2.L1
n.e.S.W.U0.R1.D2.L1
n.e.S.W.U1.R2.D2.L1
n.E.S.W.U1.R2.D2.L1
n.E.s.W.U1.R2.D2.L1
n.E.s.W.U2.R1.D0.L2
n.E.S.W.U2.R1.D0.L2
n.E.S.w.U2.R1.D0.L2
N.E.S.w.U2.R1.D0.L1
N.E.S.W.U2.R1.D0.L1
('L', 0) n.E.s.W.U2.R1.D0.L1   ************
n.E.S.W.U2.R1.D0.L1
N.e.S.w.U2.R1.D0.L1
('B', 0) N.e.S.w.U1.R0.D1.L0   ************
N.e.S.w.U0.R1.D2.L1
N.E.S.W.U0.R1.D2.L1
('R', 0) n.E.s.W.U0.R1.D2.L1   ************
'''

from servoCoding.DataTypes import Orientation
from ServoController import execute

def Move_to_face1() -> Orientation:
    '''
    ('U', 3) n.E.s.W.U2.R1.D0.L1   ************
    '''
    '''
    next_states = [
        RobotState.from_string("n.e.s.w.U0.R0.D0.L0"),
        RobotState.from_string("n.e.S.w.U2.R1.D0.L1"),
        RobotState.from_string("n.E.S.W.U2.R1.D0.L1"),
        RobotState.from_string("n.E.s.W.U2.R1.D0.L1"),
    ]'''
    for i in [
        "n.e.s.w.U0.R0.D0.L0",
        "n.e.S.w.U2.R1.D0.L1",
        "n.E.S.W.U2.R1.D0.L1",
        "n.E.s.W.U2.R1.D0.L1",
    ]: execute(i)
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('U', 3)

def Move_to_face2() -> Orientation:
    '''
    n.E.S.W.U2.R1.D0.L1
    N.e.S.w.U2.R1.D0.L1
    ('F', 3) N.e.S.w.U1.R2.D1.L2   ************
    '''
    for i in [
        "n.E.S.W.U2.R1.D0.L1",
        "N.e.S.w.U2.R1.D0.L1",
        "N.e.S.w.U1.R2.D1.L2",
    ]: execute(i)
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('F', 3)

def Move_to_face3() -> Orientation:
    '''
    N.e.S.w.U0.R1.D2.L1
    N.E.S.W.U0.R1.D2.L1
    ('D', 3) n.E.s.W.U0.R1.D2.L1   ************
    '''
    for i in [
        "N.e.S.w.U0.R1.D2.L1",
        "N.E.S.W.U0.R1.D2.L1",
        "n.E.s.W.U0.R1.D2.L1",
    ]: execute(i)
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('D', 3)

def Move_to_face4() -> Orientation:
    '''
    n.E.S.W.U0.R1.D2.L1
    n.e.S.W.U0.R1.D2.L1
    n.e.S.W.U1.R2.D2.L1
    n.E.S.W.U1.R2.D2.L1
    n.E.s.W.U1.R2.D2.L1
    n.E.s.W.U2.R1.D0.L2
    n.E.S.W.U2.R1.D0.L2
    n.E.S.w.U2.R1.D0.L2
    N.E.S.w.U2.R1.D0.L1
    N.E.S.W.U2.R1.D0.L1
    ('L', 0) n.E.s.W.U2.R1.D0.L1   ************
    '''
    for i in [
        "n.E.S.W.U0.R1.D2.L1",
        "n.e.S.W.U0.R1.D2.L1",
        "n.e.S.W.U1.R2.D2.L1",
        "n.E.S.W.U1.R2.D2.L1",
        "n.E.s.W.U1.R2.D2.L1",
        "n.E.s.W.U2.R1.D0.L2",
        "n.E.S.W.U2.R1.D0.L2",
        "n.E.S.w.U2.R1.D0.L2",
        "N.E.S.w.U2.R1.D0.L1",
        "N.E.S.W.U2.R1.D0.L1",
        "n.E.s.W.U2.R1.D0.L1",
    ]: execute(i)
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('L', 0)

def Move_to_face5() -> Orientation:
    '''
    n.E.S.W.U2.R1.D0.L1
    N.e.S.w.U2.R1.D0.L1
    ('B', 0) N.e.S.w.U1.R0.D1.L0   ************
    '''
    for i in [
        "n.E.S.W.U2.R1.D0.L1",
        "N.e.S.w.U2.R1.D0.L1",
        "N.e.S.w.U1.R0.D1.L0",
    ]: execute(i)
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('B', 0)

def Move_to_face6() -> Orientation:
    '''
    N.e.S.w.U0.R1.D2.L1
    N.E.S.W.U0.R1.D2.L1
    ('R', 0) n.E.s.W.U0.R1.D2.L1   ************
    '''
    for i in [
        "N.e.S.w.U0.R1.D2.L1",
        "N.E.S.W.U0.R1.D2.L1",
        "n.E.s.W.U0.R1.D2.L1",
    ]: execute(i)
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('R', 0)

def Move_to_faceN(n: int) -> Orientation|None:
    match (n):
        case 0: return Move_to_face1()
        case 1: return Move_to_face2()
        case 2: return Move_to_face3()
        case 3: return Move_to_face4()
        case 4: return Move_to_face5()
        case 5: return Move_to_face6()
        case _: raise ValueError(f"Move_to_faceN: can only take [0,5] but got {n}")