from DataTypes import State, Orientation, RobotState, ArmState

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
#               R        L        U         D
# (('R', 0), (90, 1), (90, 1), (0, 0), (180, 0))                  U             R                D               L
Robot_start_state = State(Orientation('R', 0), RobotState(ArmState(0, 0), ArmState(1, 1), ArmState(0, 2), ArmState(1, 1)))