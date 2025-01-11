'''
              e        w        n        s

     O        R        L        U        D
(('U', 1), (90, 1), (90, 1), (90, 0), (180, 0)) | U'.De Re'.Le'.Ue y.L
(('F', 1), (90, 0), (180, 0), (90, 1), (90, 1)) | y.L' Re.Le.Ue' U'.De'
(('D', 3), (90, 1), (90, 1), (90, 0), (0, 0)) | U.De Re'.Le' L Re.Le.U' U.De' x.U'.D2 U'.De Re'.U R'.U' Re.U U'.De'
(('L', 2), (90, 1), (90, 1), (0, 0), (180, 0)) | De Re'.Le'.Ue y.L
(('B', 2), (90, 0), (180, 0), (90, 1), (90, 1)) | y.L' Re.Le Ue'.De'
(('R', 2), (90, 1), (90, 1), (180, 0), (0, 0)) | 


('U', 1) n.E.s.W.U1.R1.D2.L1   ************
n.E.S.W.U0.R1.D2.L1
N.e.S.w.U0.R1.D2.L1
('F', 1) N.e.S.w.U1.R1.D1.L2   ************
N.e.S.w.U2.R1.D0.L1
n.E.S.W.U2.R1.D0.L1
('D', 1) n.E.s.W.U1.R1.D0.L1   ************
n.E.S.W.U2.R1.D0.L1
n.e.S.w.U2.R1.D0.L1
n.e.S.w.U2.R1.D0.L2
n.E.S.W.U1.R1.D0.L2
n.E.s.W.U2.R1.D0.L2
n.E.s.W.U1.R2.D2.L1
n.E.S.W.U0.R2.D2.L1
n.e.S.W.U1.R2.D2.L1
n.e.S.W.U0.R1.D2.L1
n.E.S.W.U1.R1.D2.L1
('L', 2) n.E.s.W.U0.R1.D2.L1   ************
n.E.S.W.U0.R1.D2.L1
N.e.S.w.U0.R1.D2.L1
('B', 2) N.e.S.w.U1.R1.D1.L2   ************
N.e.S.w.U2.R1.D0.L1
N.E.S.W.U2.R1.D0.L1
('R', 2) n.E.s.W.U2.R1.D0.L1   ************
'''

from servoCoding.DataTypes import Orientation, RobotState, State
from Command_Arduino import push_robotNums_to_arduino
from servoCoding.Optimizer import calc_weight_of_path
import time

DefO = Orientation('F', 0)
def Move_to_face1() -> Orientation:
    '''
    ('U', 1) n.E.s.W.U1.R1.D2.L1   ************
    '''
    #next_states = [
    #    RobotState.from_string("n.E.s.W.U1.R1.D2.L1"),
    #]
    #push_robotNums_to_arduino([i.as2B() for i in next_states])
    #time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('U', 1)

def Move_to_face2() -> Orientation:
    '''
    n.E.S.W.U0.R1.D2.L1
    N.e.S.w.U0.R1.D2.L1
    ('F', 1) N.e.S.w.U1.R1.D1.L2   ************
    '''
    next_states = [
        RobotState.from_string("n.E.S.W.U0.R1.D2.L1"),
        RobotState.from_string("N.e.S.w.U0.R1.D2.L1"),
        RobotState.from_string("N.e.S.w.U1.R1.D1.L2"),
    ]
    push_robotNums_to_arduino([i.as2B() for i in next_states])
    time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('F', 1)

def Move_to_face3() -> Orientation:
    '''
    N.e.S.w.U2.R1.D0.L1
    n.E.S.W.U2.R1.D0.L1
    ('D', 1) n.E.s.W.U1.R1.D0.L1   ************
    '''
    next_states = [
        RobotState.from_string("N.e.S.w.U2.R1.D0.L1"),
        RobotState.from_string("n.E.S.W.U2.R1.D0.L1"),
        RobotState.from_string("n.E.s.W.U1.R1.D0.L1"),
    ]
    push_robotNums_to_arduino([i.as2B() for i in next_states])
    time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('D', 1)

def Move_to_face4() -> Orientation:
    '''
    n.E.S.W.U2.R1.D0.L1
    n.e.S.w.U2.R1.D0.L1
    n.e.S.w.U2.R1.D0.L2
    n.E.S.W.U1.R1.D0.L2
    n.E.s.W.U2.R1.D0.L2
    n.E.s.W.U1.R2.D2.L1
    n.E.S.W.U0.R2.D2.L1
    n.e.S.W.U1.R2.D2.L1
    n.e.S.W.U0.R1.D2.L1
    n.E.S.W.U1.R1.D2.L1
    ('L', 2) n.E.s.W.U0.R1.D2.L1   ************
    '''
    next_states = [
        RobotState.from_string("n.E.S.W.U2.R1.D0.L1"),
        RobotState.from_string("n.e.S.w.U2.R1.D0.L1"),
        RobotState.from_string("n.e.S.w.U2.R1.D0.L2"),
        RobotState.from_string("n.E.S.W.U1.R1.D0.L2"),
        RobotState.from_string("n.E.s.W.U2.R1.D0.L2"),
        RobotState.from_string("n.E.s.W.U1.R2.D2.L1"),
        RobotState.from_string("n.E.S.W.U0.R2.D2.L1"),
        RobotState.from_string("n.e.S.W.U1.R2.D2.L1"),
        RobotState.from_string("n.e.S.W.U0.R1.D2.L1"),
        RobotState.from_string("n.E.S.W.U1.R1.D2.L1"),
        RobotState.from_string("n.E.s.W.U0.R1.D2.L1"),
    ]
    push_robotNums_to_arduino([i.as2B() for i in next_states])
    time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('L', 2)

def Move_to_face5() -> Orientation:
    '''
    n.E.S.W.U0.R1.D2.L1
    N.e.S.w.U0.R1.D2.L1
    ('B', 2) N.e.S.w.U1.R1.D1.L2   ************
    '''
    next_states = [
        RobotState.from_string("n.E.S.W.U0.R1.D2.L1"),
        RobotState.from_string("N.e.S.w.U0.R1.D2.L1"),
        RobotState.from_string("N.e.S.w.U1.R1.D1.L2"),
    ]
    push_robotNums_to_arduino([i.as2B() for i in next_states])
    time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('B', 2)

def Move_to_face6() -> Orientation:
    '''
    N.e.S.w.U2.R1.D0.L1
    N.E.S.W.U2.R1.D0.L1
    ('R', 2) n.E.s.W.U2.R1.D0.L1   ************
    '''
    next_states = [
        RobotState.from_string("N.e.S.w.U2.R1.D0.L1"),
        RobotState.from_string("N.E.S.W.U2.R1.D0.L1"),
        RobotState.from_string("n.E.s.W.U2.R1.D0.L1"),
    ]
    push_robotNums_to_arduino([i.as2B() for i in next_states])
    time.sleep(calc_weight_of_path([State(DefO, i) for i in next_states]))
    return Orientation('R', 2)