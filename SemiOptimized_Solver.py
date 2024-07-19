'''
2 major changes:
 - We make a 5x5x5 array, where the 5 layer around the core stores face-numbers 
    and the 3 layer around the core stores bitmasked final destinations.
 - URFLBD reigns supreme over the entire cube

'''
import numpy as np
import time


########################################################################################################
#********* THIS IS THE ERA OF URFLBD (a universal acronym and ordering to map faces to numbers) *******#
########################################################################################################
faces = "URFLBD"
faces_to_nums = {'U': 0,
                 'R': 1,
                 'F': 2,
                 'L': 3,
                 'B': 4,
                 'D': 5}


#####################################  CUBE DATA STRUCTURE(S)  ##########################################
# But first, lets get our 2nd data structure type. 
CUBE = {'U': np.array([['L', 'L', 'L'], 
                      ['F', 'U', 'R'], 
                      ['R', 'R', 'D']]),

        'R': np.array([['L', 'U', 'F'], 
                      ['F', 'R', 'L'], 
                      ['B', 'F', 'B']]),

        'F': np.array([['D', 'D', 'F'], 
                      ['D', 'F', 'R'], 
                      ['B', 'F', 'R']]),

        'L': np.array([['B', 'L', 'F'], 
                      ['B', 'L', 'B'], 
                      ['R', 'U', 'D']]),  

        'B': np.array([['U', 'B', 'U'], 
                      ['D', 'B', 'D'], 
                      ['R', 'B', 'U']]),

        'D': np.array([['U', 'U', 'F'], 
                      ['U', 'D', 'L'], 
                      ['D', 'D', 'L']])}
#################################  GENERIC OPERATION CORRESPONDANCES  #######################################
                                    # U  R  F  L  B  D
move_from_Fs_perspective = np.array([[4, 1, 0, 3, 5, 2],  # U
                                     [0, 4, 1, 2, 3, 5],  # R
                                     [0, 1, 2, 3, 4, 5],  # F
                                     [0, 2, 3, 4, 1, 5],  # L
                                     [0, 3, 4, 1, 2, 5],  # B
                                     [4, 3, 5, 1, 0, 2]]) # D



######################################  ALGORITHMS  ###############################################
piece_indexes = np.array([[[ 0,  0,  0,  0,  0],
                            [ 0, 21, 22, 13,  0],
                            [ 0, 15,  0,  9,  0],
                            [ 0, 10,  7,  0,  0],
                            [ 0,  0,  0,  0,  0]],

                            [[ 0, 23, 23, 14,  0],
                            [22,  0,  0,  0, 12],
                            [14,  0,  0,  0,  8],
                            [ 9,  0,  0,  0,  1],
                            [ 0, 11,  6,  2,  0]],

                            [[ 0, 21,  0, 17,  0],
                            [20,  0,  0,  0, 16],
                            [ 0,  0,  0,  0,  0],
                            [ 5,  0,  0,  0,  1],
                            [ 0,  4,  0,  0,  0]],

                            [[ 0, 20, 19, 17,  0],
                            [18,  0,  0,  0, 16],
                            [12,  0,  0,  0, 10],
                            [ 7,  0,  0,  0,  3],
                            [ 0,  8,  2,  5,  0]],

                            [[ 0,  0,  0,  0,  0],
                            [ 0, 19, 18, 15,  0],
                            [ 0, 13,  0, 11,  0],
                            [ 0,  6,  3,  4,  0],
                            [ 0,  0,  0,  0,  0]]], dtype=np.uint8)
FULL_CROSS = ("U2 F2", "U R' F R", "U F2", "R' F R", "F2", "U' R' F R", "U' F2", "L F' L'", 
            "D' L' D", "L2 F' L2", "D R D'", "R2 F R2", "D R' D'", "F", "D' L D", "F'", 
            "D' R D F", "B2 U2 F2", "R F", "R2 U F2", "D R D' F", "", "L' F'", "L2 U' F2")
FULL_F2L = [[["R2 U2 R' U' R U' R2", "F' L' U' L2 F' L' F2"], ["F2 U2 R' F' R U2 F2", "F2 L2 B L B' L F2"], ["R B' R B R2"], ["U F' U2 F2 R' F' R", "F' U2 L' U2 L U' F", "U F' U' L F' L' F2"], ["U' R U2 R2 F R F'", "U' R U B' R B R2", "R U2 B U2 B' U R'"], ["F' L F' L' F2"], ["R2 U2 F R F' U2 R2", "R2 B2 L' B' L B' R2"], ['R B U B2 R B R2', "F2 U2 F U F' U F2"], ["U2 F2 D' L2 D F2", "U2 R2 D B2 D' R2"], ["F' L U' L' F", "R B' U B R'"], ["U R2 F R2 F'"], ["F' U' F2 D R D' F'", "R2 U R B U' B' R2", "R B' R' B R U R'"], ["R2 U2 R2 U' R2 U' R2", "L' U R2 U' L U R2", "F' U L' U' L U' F", "R U' B U B' U R'", "B U' F2 U B' U' F2", 'F2 U2 F2 U F2 U F2'], ["R B U' B2 R B R2", "U' R U2 R' F' U' F", "U2 R U' R' F' U2 F", "U F' U2 F R U R'", "U2 F' U F R U2 R'", "F' L' U L2 F' L' F2"], ["U' F2 R' F2 R"], ["R U R2 D' F' D R", "F' L F L' F' U' F", "F2 U' F' L' U L F2"]],
[["U' F' U2 F U' R U R'", "U2 R U' B U2 B' U2 R'", "U2 R' U2 R2 U R2 U R"], ["F' U2 F"], ["L F' R' F' R F L'", "R U' R' U' R U R'", "U2 F2 U' L' U L F2"], ["F' L' U' L U2 F"], ["U2 R U R'"], ["B' F' U2 B L2 F' L2 F2", "U2 B' R' U' R2 U R' B", "U2 L' B2 R2 B R2 B L", "F2 L2 F' R' F L2 F2 R", "U2 R2 U2 F R2 F' U2 R2", "F R2 F' U2 F' U2 F R2", "F R2 B' R2 F' R2 B R2", "B U2 B' U2 B' R2 B R2", "U' F' U2 L' U L U' F", "U2 F' U' L' U2 L U' F", "U' F' U2 F U2 F' U F", "U' F' U2 F U F' U2 F", "U F' U' L' U2 L U2 F", "F2 L2 F L2 B' U2 B F"], ["R2 U R' U R U2 R2"], ["U2 R U' B U' B' U2 R'", "U2 R2 B' D B' D' B2 R2", "U2 R U' R' U2 F' U' F", "U2 F' U2 L' U' L U' F", "R U R' F' L' U2 L F", "U2 F' L' B' U B L F"], ["R U R D B2 D' R2", "U B' U B F' U F", "U B' F' U B U F", "F' U B' U B U F", "U L U R U L' R'", "U L U L' R U R'", 'D2 F2 L2 F L2 F D2', "D2 B' R2 B' R2 B2 D2"], ["U2 B' R U R' B"], ["U2 B' R2 B R2"], ["U2 B R U R' B'", "F' U2 R' U R F", "U2 F D R D' F'"], ["F' L' U2 L U2 F"], ["F' U' F R' F R F'", "F' U' F U R U' R'", "F' U F U R U R'", "F' U' F U2 R U2 R'"], ["L F L' F' L F' L'", "F' L R' F2 R F L'", "F' U2 F2 L F2 L' F'", "F2 U' F' U F U' F", "F' U' F2 U F2 U' F", "L' U' L2 F' L' U' F", "F' L R' F2 L' R F", "U L' U R U L R'", "U L' U' L R U' R'", "R U L' U' L U' R'", "L' U' L U2 R U R'", "U L' U L R U R'", "F' U2 F' U F2 U' F2", "F' U2 F' U2 F2 U2 F2", "L F R' F L' R F2", "F' U' F L F2 L' F2"], ["L' U L F' U2 F"]],
[["U2 F2 L D' L D L2 F2", "U2 F' U F U2 R U R'", "U2 R U2 B U B' U R'", "F' U' F R B U2 B' R'", "U2 R B L U' L' B' R'", "U2 F' U L' U L U2 F"], ["F2 U' F U' F' U2 F2"], ["U2 F2 U2 R' F2 R U2 F2", "R' F2 R U2 R U2 R' F2", "R' F2 L F2 R F2 L' F2", "L' U2 L U2 L F2 L' F2", "U R U2 R' U2 R U' R'", "U R U2 B U' B' U R'", "U2 R U B U2 B' U R'", "U R U2 R' U' R U2 R'", "U' R U B U2 B' U2 R'", "U2 L F U F2 U' F L'", "R2 B2 R F R' B2 R2 F'", "L R U2 L' B2 R B2 R2", "R2 B2 R' B2 L U2 L' R'", "U2 B L2 F2 L' F2 L' B'"], ["U2 F' U' F"], ["R B U B' U2 R'"], ["U2 R2 U B U' B' R2", "F' U F U F' U' F", "B' R F R F' R' B"], ["R U2 R'"], ["U2 F U2 F2 U' F2 U' F'", "U R U2 R' U F' U' F", "U2 F' U L' U2 L U2 F"], ["U' B' U' B F' U' F", "U' B' U' F' U' B F", "U' L U' L' R U' R'", "U' L R U' L' U' R'", "R U' L U' L' U' R'", "F' U' F' D' L2 D F2", "D2 R2 B2 R' B2 R' D2", 'D2 L F2 L F2 L2 D2'], ["U2 L F' U' F L'"], ["R U2 R U' R2 U R2", 'R U2 R U2 R2 U2 R2', "B' R' F R' B F' R2", "R U R' B' R2 B R2", "R U2 R2 B' R2 B R", "B U B' U2 F' U' F", "U' B U' B' F' U' F", "U' B U B' F' U F", "F' U' B U B' U F", "R B' F R2 F' R' B", "B' R' B R B' R B", "U' B U' F' U' B' F", "R2 U R U' R' U R'", "R U R2 U' R2 U R'", "B U B2 R B U R'", "R B' F R2 B F' R'"], ["B U' B' R U2 R'"], ["R B U2 B' U2 R'"], ["R U R' F R' F' R", "R U' R' U' F' U' F", "R U R' U' F' U F", "R U R' U2 F' U2 F"], ["U2 L F2 L' F2"], ["R U2 F U' F' R'", "U2 L' F' U' F L", "U2 R' D' F' D R"]],
[["U R U2 R' U' R U R'", "U' R2 U2 F R F' U2 R2", "U' R2 B2 L' B' L B' R2"], ["U' F2 U2 F U F' U F2", "R B' R B R F R F'", "U' R B U B2 R B R2", "R' F R F' U' F' U' F"], ["F' U' L' U' L2 F' L' F2", "U' F' L' U' L2 F' L' F2", "F' U2 F' U' L' U L F2", "U' R2 U2 R' U' R U' R2", "R B U' B' R' F' U' F"], ["U' F2 U2 R' F' R U2 F2", "U' F2 L2 B L B' L F2", "U F' U2 F U F' U' F"], ["U R B U2 B' R'", "U' R B' R B R2"], ["F' U' L F' L' F2", "F' U2 F2 R' F' R"], ["U2 R U2 R2 F R F'", "U2 R U B' R B R2", "R2 B' R' B2 U2 B' R'"], ["U' F' L F' L' F2", "F' L' U' L U' F", "U F' L' U2 L F"], ["U F2 D' L2 D F2", "U R2 D B2 D' R2"], ["U' R B' U B R'", "F' U' L U' L' F", "U' F' L U' L' F"], ["R2 F R2 F'"], ["F' U2 F2 D R D' F'"], ["F' U L' U2 L U' F"], ["F' U2 F R U R'"], ["U2 F2 R' F2 R"], ["L' U L2 F' L' U2 F"]],
[["R2 D R' U R D' R2", "R U' R' U R U R'"], ["U R U' B U' B' U2 R'", "U R2 B' D B' D' B2 R2", "F R' F' R U' F' U' F", "U R U' R' U2 F' U' F", "U F' U2 L' U' L U' F", "R2 B' R2 F' R B R' F", "U F' L' B' U B L F"], ["U2 F' U2 F U' R U R'", "U R U' B U2 B' U2 R'", "U R' U2 R2 U R2 U R"], ["F' U F"], ["F' U' L' U L U2 F", "R U R' U R U R'", "F' U' F U2 R U R'", "U F2 U' L' U L F2"], ["L F' R' F R F L'", "U R2 B U B' U' R2", "U' F' L' U' L U2 F", "F2 U2 R' F R U2 F2"], ["U R U R'"], ["F' U' L' U2 L U2 F"], ["L U L' R U R'", "L U R U L' R'", "B' U B F' U F", "B' F' U B U F"], ["U B' R U R' B"], ["U B' R2 B R2"], ["U B R U R' B'", "F' U R' U R F", "U F D R D' F'"], ["L D' L' U2 L D L'", "R' D' R U R' D R", "B' U F' U' B U F", "F' U' L' U' L U2 F", "U' F' L' U2 L U2 F", "B D2 B' U' B D2 B'", "R U2 R' U R U R'", "U2 R U2 B U2 B' R'"], ["F' U2 F' L' U' L U F2", "U' F' U' F U R U' R'", "U2 F' U' F U' R U R'", "U' F' U F U R U R'", "U' F' U' F U2 R U2 R'", "U' F' U' F R' F R F'", "U2 F' L F' L' F U' F", "F2 U' L' U L F U2 F"], ["L' U' L R U' R'", "L' U L R U R'", "L' U R U L R'"], ["F' U R' F' R U F", "U' L' U L F' U2 F", "U F U2 F' R U R'", "U R U F U' F' R'"]],
[["U2 R U' R'", "U' R U2 R'"], ["R U2 R' U F' U' F"], ["R2 D R D' R F2 U F2", "U F2 L D' L D L2 F2", "U F' U F U2 R U R'", "F' L' U' L F R U R'", "U R U2 B U B' U R'", "F' U2 F R B U2 B' R'", "U R B L U' L' B' R'", "U F' U L' U L U2 F", "F' R' F' U' F U R F"], ["U' F2 U' F U' F' U2 F2", "U2 F2 D' F U' F' D F2", "U2 F' U F U' F' U' F"], ["R U2 R' U2 R U' R'", "R U2 B U' B' U R'", "R U2 R' U' R U2 R'"], ["U F' U' F"], ["R U R' U2 R U' R'", "R U B U' B' U R'", "R U R' U' R U2 R'", "U' R B U B' U2 R'", "U F2 L' U' L U F2"], ["U R2 U B U' B' R2", "R U' R' U F' U' F"], ["R2 B R' B2 R B' R2", "B' U2 B U F' U' F", "U2 B' U' B F' U' F", "U2 B' U' F' U' B F", "U2 L U' L' R U' R'", "U2 L R U' L' U' R'", "F' U2 F' D' L2 D F2"], ["U L F' U' F L'"], ["U2 B U' B' F' U' F", "U2 B U B' F' U F", "F' U2 B U B' U F", "F' R2 D' F' D R2 F", "U2 B U' F' U' B' F", "R' F' U' F R2 U R'"], ["U R' U2 R F' U' F", "U F' U' R' U R F", "U' B U' B' R U2 R'"], ["F' U2 L' U2 L F"], ["R U R' U F' U' F", "R B' R B R' U R'"], ["U L F2 L' F2"], ["U L' F' U' F L", "U R' D' F' D R"]],
[["U R U2 R2 F R F'", "U R U B' R B R2"], ["F' L' U2 L F"], ["R U2 R' U' R U R'"], ["U2 F2 U2 F U F' U F2", 'U2 R B U B2 R B R2', "F' U' F' L F L' U2 F"], ["U2 F' L' U' L2 F' L' F2", "R U R B' R' B U2 R'", "U2 R2 U2 R' U' R U' R2"], ["F' U2 F U F' U' F"], ["R B U2 B' R'"], ["U' F' U2 F2 R' F' R", "U' F' U' L F' L' F2"], ["F2 D' L2 D F2", "R2 D B2 D' R2"], ["U2 R B' U B R'", "U2 F' L U' L' F"], ["R F R2 F' R'", "U' R2 F R2 F'", "R2 U R2 U' R2"], ["B' R B2 U' B' U2 R'"], ['R2 U R2 U R2 U2 R2', "F2 U' F2 U' F2 U2 F2"], ["F' U F R U2 R'", "R U' R' F' U2 F"], ["F' R' F2 R F", "F2 U' F2 U F2", "U F2 R' F2 R"], ["L F' L2 U L U2 F"]],
[["R U R'"], ["R2 U2 F R2 F' U2 R2", "L' B2 R2 B R2 B L", "F' U' L' U2 L U' F", "B' R' U' R2 U R' B"], ["F' U L' U L2 F' L' F2", "U' R U' R' U R U R'", "F' U' F2 R' F' R2 U R'", "R B L' B L B2 U2 R'", "U2 R2 U R' U R U2 R2", "R B' U' R' U R2 B R2", "F R B' R' F' R2 B R2", "F' R B' R' F R2 B R2", "U' R2 D R' U R D' R2"], ["R2 B' D B' D' B2 R2", "R U' R' U2 F' U' F", "F' U2 L' U' L U' F", "F' L' B' U B L F", "R U' B U' B' U2 R'"], ["R' U2 R2 U R2 U R", "R U' B U2 B' U2 R'"], ["F R' F' R", "U' F' U F", "U2 F' U2 F"], ["F2 U' L' U L F2"], ["R2 B U B' U' R2"], ["F L2 U2 F' U2 L2 F'", "U' B' U B F' U F", "U' B' F' U B U F", "B2 R' U' R2 U R' B2", "B2 R D' R2 D R B2", "U' L U R U L' R'", "B D2 L2 F' L2 D2 B'", "R U2 L U' L' U' R'", "U' L U L' R U R'", "F2 L' D' L2 D L' F2", "F2 L U' L2 U L F2"], ["B' R U R' B"], ["B' R2 B R2"], ["F D R D' F'", "B R U R' B'"], ["L D' L' U L D L'", "U2 F' L' U2 L U2 F", "B D2 B' U2 B D2 B'", "U R U2 B U2 B' R'"], ["U2 F' U' F U R U' R'", "U F' U' F U' R U R'", "U2 F' U F U R U R'", "U2 F' U' F U2 R U2 R'", "U2 F' U' F R' F R F'", "U F' L F' L' F U' F"], ["R2 B2 L2 B' L2 B' R2", "U' L' U R U L R'", "U' L' U' L R U' R'", "U' L' U L R U R'"], ["F U2 F' R U R'", "R U F U' F' R'"]],
[["F2 L' U' L U F2"], ["R2 U B U' B' R2"], ["U R U' R'", "U2 R U2 R'", "R' F R F'"], ["F U2 F2 U' F2 U' F'", "F' U L' U2 L U2 F"], ["F' U L' U L U2 F", "F' U F U2 R U R'", "R U2 B U B' U R'", "R B L U' L' B' R'", "F2 L D' L D L2 F2"], ["U2 F2 U' F U' F' U2 F2", "F' L U F U' F2 L' F2", "R F' L F R' F2 L' F2", "R' F' L F R F2 L' F2", "U F2 D' F U' F' D F2", "R U' B U' B2 R B R2", "U F' U F U' F' U' F", "R U R2 F R F2 U' F", "F' L' B L' B' L2 U2 F"], ["L F U F2 U' F L'", "B L2 F2 L' F2 L' B'", "R U B U2 B' U R'", "F2 U2 R' F2 R U2 F2"], ["F' U' F"], ["R2 B' U B2 U' B' R2", "R2 B D B2 D' B R2", "L2 F' D F2 D' F' L2", "L2 F U F2 U' F L2", "L' D2 B2 R B2 D2 L", "R' B2 U2 R U2 B2 R", "U B' U' B F' U' F", "F' U2 B' U B U F", "U B' U' F' U' B F", "U L U' L' R U' R'", "U L R U' L' U' R'"], ["L F' U' F L'"], ["U B U' B' F' U' F", "U B U B' F' U F", "U B U' F' U' B' F", 'F2 L2 B2 L B2 L F2'], ["R' U2 R F' U' F", "F' U' R' U R F"], ["L' D2 L U2 L' D2 L", "U' F' U2 L' U2 L F", "B' D B U' B' D' B", "U2 R B U2 B' U2 R'"], ["U' R B' R B R' U R'", "U2 R U R' F R' F' R", "U2 R U' R' U' F' U' F", "U' R U R' U F' U' F", "U2 R U R' U' F' U F", "U2 R U R' U2 F' U2 F"], ["L F2 L' F2"], ["R' D' F' D R", "L' F' U' F L"]],
[["R B U B' U R'", "U' R B U2 B' R'", "U R B' R B R2"], ["U2 F' U2 F2 R' F' R", 'F2 L F L2 U2 L F', "U2 F' U' L F' L' F2"], ["R U B' R B R2", "R U2 R2 F R F'"], ["U F' L F' L' F2", "U' F' L' U2 L F"], ["U' R U2 R' U' R U R'", "U R2 U2 F R F' U2 R2", "U R2 B2 L' B' L B' R2"], ["U F2 U2 F U F' U F2", "F' L' U L F R U R'", "R U2 R U B U' B' R2", 'R U B U B2 R B R2', 'U R B U B2 R B R2'], ["U F' L' U' L2 F' L' F2", "F R' F' R U R U R'", "F' L F' L' F' R' F' R", "U R2 U2 R' U' R U' R2"], ["U F2 U2 R' F' R U2 F2", "U F2 L2 B L B' L F2", "U' F' U2 F U F' U' F"], ["U' F2 D' L2 D F2", "U' R2 D B2 D' R2"], ["R U B' U B R'", "U R B' U B R'", "U F' L U' L' F"], ["U2 R2 F R2 F'"], ["B U' B2 R B U2 R'"], ["R U' B U2 B' U R'"], ["R U2 R' F' U' F"], ["F2 R' F2 R"], ["R U2 R2 D' F' D R"]],
[["F' U F U' R U R'", "U' F2 U' L' U L F2"], ["U' R2 B U B' U' R2", "F' U' L' U L U' F", "F' U' F U2 F' U F", "F' U' F U F' U2 F", "U F' L' U' L U2 F"], ["U' R U R'"], ["F' U2 L' U L U' F", "F' U2 F U2 F' U F", "F' U2 F U F' U2 F"], ["U2 R U' R' U R U R'", "U R2 U R' U R U2 R2", "U2 R2 D R' U R D' R2"], ["U' R U' B U' B' U2 R'", "R F R U R' U' F' R'", "F2 D' F' D F' R2 U' R2", "U' R2 B' D B' D' B2 R2", "U' R U' R' U2 F' U' F", "R B U B' R' F' U' F", "U' F' U2 L' U' L U' F", "R U2 R' F' L' U2 L F", "U' F' L' B' U B L F"], ["F' U2 F U' R U R'"], ["U2 F' U F", "U F' U2 F"], ["R U2 R D B2 D' R2", "U2 B' U B F' U F", "U2 B' F' U B U F", "U2 L U R U L' R'", "L U2 L' U' R U R'", "U2 L U L' R U R'", "F2 L' F L2 F' L F2"], ["U' B' R U R' B"], ["U' B' R2 B R2"], ["U' B R U R' B'", "U' F D R D' F'"], ["R U2 B U2 B' R'"], ["F' L F' L' F U' F", "F' U' F U' R U R'"], ["F R U R' F2 U' F", "U2 L' U R U L R'", "U2 L' U' L R U' R'", "R U2 L' U' L U' R'", "U2 L' U L R U R'", "R F2 D R D' F2 R'"], ["U L' U L F' U2 F", "U' F U2 F' R U R'", "U' R U F U' F' R'"]],
[["R U B U2 B' U2 R'"], ["U' F' U' F"], ["R2 U2 F R' F' U2 R2", "B' R F R' F' R' B", "U R B U B' U2 R'", "U' F2 L' U' L U F2"], ["U' R2 U B U' B' R2", "F' U' F U' F' U' F", "R U R' U2 F' U' F", "R U B U' B' U2 R'"], ["R U' R'"], ["U' F U2 F2 U' F2 U' F'", "U2 R U2 R' U F' U' F", "U' F' U L' U2 L U2 F"], ["U' F2 L D' L D L2 F2", "R' F R F' U R U R'", "U' F' U F U2 R U R'", "U' R U2 B U B' U R'", "F2 L F2 R F' L' F R'", "U' R B L U' L' B' R'", "U' F' U L' U L U2 F"], ["F' U F U' F' U' F", "F2 D' F U' F' D F2"], ["L U' L' R U' R'", "L R U' L' U' R'", "B' U' F' U' B F", "B' U' B F' U' F"], ["U' L F' U' F L'"], ["B U' F' U' B' F", "B U' B' F' U' F", "B U B' F' U F"], ["U' R' U2 R F' U' F", "U' F' U' R' U R F", "R U' F R F' U' R'", "U B U' B' R U2 R'"], ["F D F' U' F D' F'", "L' D2 L U L' D2 L", "F' U2 F U' F' U' F", "U2 F' U2 L' U2 L F", "B' D B U2 B' D' B", "L U' R U L' U' R'", "R U B U B' U2 R'", "U R B U2 B' U2 R'"], ["U2 R B' R B R' U R'", "R2 U B U' B' R' U2 R'", "U R U R' F R' F' R", "R U2 R B U B' U' R2", "U R U' R' U' F' U' F", "U2 R U R' U F' U' F", "U R U R' U' F' U F", "U R U R' U2 F' U2 F"], ["U' L F2 L' F2"], ["R U' F U' F' R'", "U' L' F' U' F L", "U' R' D' F' D R"]],
[["B' U2 B R U R'"], ["U L U2 F' U' F L'", "L2 F' L2 U2 L F L'", "L2 F' L2 U2 L2 F L2", "U' L U2 L' F' U' F", "U L U' L' F' U2 F", "F' L2 F' L2 F U2 F", "F' U L U' L' U2 F", "U F' L U' L' U2 F", "B' U2 F' U2 B U2 F", "B' U2 B' R2 B R2 B", 'D2 B2 R2 B R2 B D2'], ["R B' R' U2 R2 B R2", "U B' U2 R U R' B", "B' R2 U R' U' R2 B", "U' B' U2 B R U R'", "U B' U B R U2 R'", "U R B' U B U2 R'", "F2 L' F2 U2 F2 L F2", 'D2 R B2 R B2 R2 D2'], ["L U' L' F' U2 F", "F' L U' L' U2 F", "L U2 F' U' F L'"], ["B' U B R U2 R'", "R B' U B U2 R'", "B' U2 R U R' B"], ["U' L U2 F' U' F L'", "L F2 U' F U F2 L'", "R2 B R2 U2 R2 B' R2", "U L U2 L' F' U' F", "U' L U' L' F' U2 F", "U' F' L U' L' U2 F", "F' L F U2 F2 L' F2", "D2 F' L2 F' L2 F2 D2"], ["L U2 L F2 L' F2 L'", "B2 R B2 U2 B2 R' B2", "U' B' U2 R U R' B", "B2 R B2 U2 B' R' B", "U B' U2 B R U R'", "R B2 R B2 R' U2 R'", "U' B' U B R U2 R'", "L U2 R U2 L' U2 R'", "R U' B' U B U2 R'", "U' R B' U B U2 R'", "D2 L2 F2 L' F2 L' D2"], ["L U2 L' F' U' F"], ['L F2 L F2 L2', 'F2 L2 F L2 F', "R2 B2 R' B2 R'", "L R U2 L' R'", "B' F' U2 B F", "B' R2 B' R2 B2"], ["B' U2 R' U2 R2 U R' B", "R B' U B2 U2 B' U2 R'", "L U2 F U2 F2 U' F L'", "F' L U' L2 U2 L U2 F"], ["B' U2 R2 B R2"], ["B2 U' R B' U R' B2", "B' U2 B2 R U R' B'", "B2 R B' U' B' U2 R'", "B U' B R B2 U2 R'", "B2 U' R B' U B2 R'"], ["L D' F U' F' D L'", "R2 B' R2 U2 R2 B R2", "L2 F L2 U2 L2 F' L2", "F' U' L U' L' U2 F", "B2 R' B2 U2 B2 R B2", "B' D R' U R D' B", "R U B' U B U2 R'", "F2 L F2 U2 F2 L' F2"], ["B2 R B U' R B' R2 B2", "B' R2 B' R2 D' R D B2", "B' F' U B F R U2 R'", "L2 F' L' U F' L F2 L2", "L F2 L F2 D F' D' L2", "L R U' L' R' F' U2 F"], ["L U2 F2 L' F2"], ["L2 U F' L U' F L2", "L U2 L2 F' U' F L", "L2 F' L U L U2 F", "L' U L' F' L2 U2 F", "L2 U F' L U' L2 F"]],
[["R U' B' U2 B R'", "U' R B' U2 B R'", "U' B' R U2 R' B"], ["B' U' B U' F' U' F", "U B' U2 B F' U' F", "U B2 F' U F U' B2", "F' U B2 U F U' B2", "B2 R' B R B' R B2", "U B2 R B U R' B", "U B' U2 F' U' B F", "L' B L B' R U2 R'", "L' R B L B' U2 R'", "D2 F' L U2 L' F D2", "D2 L F' U2 F L' D2"], ["U2 R B' U2 B R'", "U2 B' R U2 R' B", "L2 U' R U L2 R'", "R B' U R B R2"], ["B2 F' U F U' B2", "B2 R B U R' B", "B' U2 F' U' B F", "B' U2 B F' U' F"], ["R U B' U2 B R'", "U R B' U2 B R'", "U B' R U2 R' B"], ["L F' L F' L' F2 L'", "L U' L' U2 F' U' F", "U' B' U2 B F' U' F", "U' B2 F' U F U' B2", "B2 R F R F' R' B2", "U' B2 R B U R' B", "U' B' U2 F' U' B F"], ["R B' U2 B R'", "B' R U2 R' B"], ["U2 B' U2 B F' U' F", "B F' L2 B' L2 U' F", "U2 B2 F' U F U' B2", "F' U2 B2 U F U' B2", "U2 B2 R B U R' B", "U2 B' U2 F' U' B F"], ["R' D' R2 U' R D B2 R2", "B2 R2 D B U' B2 D' B'"], ["B2 R B R' B R U R'", "B' U B R B' R B R2", "D2 F' L U2 L' U F D2", "D2 F' U' L U' L' F D2", "D2 L F' U' F U' L' D2", "D2 L U F' U2 F L' D2"], ["B' R U R B R2", 'B2 R B R B R2'], ["B U' B2 R U2 R' B", "B U' B2 R U2 B R'"], ["R U2 B' U2 B R'", "R2 U' L U L' R2", "B' F2 U B U' F2"], ["F' U B' F R U2 R' B", "B2 R2 B U R U' R2 B", "L2 D F' D' L2 R U' R'", "R U' L' B L B' U2 R'", "F' U F R B' U2 B R'", "L2 D F D' F2 L' F2 L'", "L U2 F' U F' L F2 L2", "R B U' B2 U R B R2", "R B2 U' B U R B2 R2", "F' U L2 F' L' F L' F"], ["F' L2 D F2 D' F' L2", "F2 L' U' F2 U F2 L", "F2 U' L' F2 U F2 L", "B' U2 L F2 L' B F2", "B' F2 U B F2 U' F2", "L U R' F2 L' R F2", "L R U R' F2 L' F2", "B' U2 B L F2 L' F2", "B' F2 U B D' L2 D"], ["F U B' F' R U2 R' B", "B' R U2 F U' F' R' B", "B' U2 R' D' F' D R B", "F U B' F L F2 L' B", "B' U2 L' F' U' F L B", "L' U B L B' R U2 R'", "L' R U B L B' U2 R'", "L' U R B L B' U2 R'", "R B' U2 B F U' F' R'", "R B' U2 F U' B F' R'", "R F U B' F' U2 B R'", "F U F' R B' U2 B R'", "F' L2 F L' F' U' F L'", "D' L D L F U' F2 L'", "F U F2 L2 F' L' F2 L'", "B' U2 B L' F' U' F L", "B' U2 B R' D' F' D R", "L2 U' B' R U R' B L2", "L' U' L2 U2 F' L' U2 F", "L U2 F' L2 U L U2 F", "F' L2 F L' F' U' L' F", "R F U' F2 L2 F L2 R'"]],
[["U2 L2 F' L' U' F L'", "U2 L2 R U' R' U L2", "R U2 L2 U' R' U L2", "U2 L U2 R U L' R'", "U2 L U2 L' R U R'", "L' R B2 L B2 U R'"], ["F' L U2 L' F", "L F' U2 F L'"], ["U L2 F' L' U' F L'", "U L2 R U' R' U L2", "L2 F' R' F' R F L2", "U L U2 R U L' R'", "B' R B' R B R2 B", "B' U B U2 R U R'", "U L U2 L' R U R'"], ["F' U' L U2 L' F", "U' F' L U2 L' F", "U' L F' U2 F L'"], ["L U2 L' R U R'", "L U2 R U L' R'", "L2 R U' R' U L2", "L2 F' L' U' F L'"], ["F' L U' F' L' F2", "B2 U F' U' B2 F", "U2 F' L U2 L' F", "U2 L F' U2 F L'"], ["U' L2 F' L' U' F L'", "U' L2 R U' R' U L2", "R U' L2 U' R' U L2", "L2 F L' F' L F' L2", "B L' B' L F' U2 F", "B F' L' B' L U2 F", "U' L U2 R U L' R'", "L U L' U R U R'", "U' L U2 L' R U R'", "D2 R B' U2 B R' D2", "D2 B' R U2 R' B D2"], ["F' U L U2 L' F", "U F' L U2 L' F", "U L F' U2 F L'"], ["F D F2 U F' D' L2 F2", "L2 F2 D' L' U L2 D L"], ["L U' L' F' L F' L' F2", "L2 F' L' F L' F' U' F", "D2 R B' U2 B U' R' D2", "D2 R U B' U B R' D2", "D2 B' R U R' U B D2", "D2 B' U' R U2 R' B D2"], ["L R2 U' L' R2 U R2", "B' U' F R2 B F' R2", "B' F' U' F R2 B R2", "L U2 L' B' R2 B R2", "R B2 D' R2 D R B2", "L U2 B' R2 B L' R2", "L R2 U' L' D B2 D'", "R2 B U R2 U' R2 B'", "R2 U B R2 U' R2 B'"], ["B2 U L F' U' F L' B2", "R B2 R' B R U R' B", "D B' D' B' R' U R2 B", "R' U' R2 B2 R B R2 B", "B' U2 R B2 U' B' U2 R'", "B U B2 U2 R B U2 R'", "R B2 R' B R U B R'", "L U2 F D R D' F' L'", "R' U' L R F' U2 F L'", "L F' U2 R' U R F L'", "L U2 B R U R' B' L'", "R' U' L R' B' R2 B L'", "L U2 L' F D R D' F'", "B U' L' B' L F' U2 F", "B F' U' L' B' L U2 F", "B U' F' L' B' L U2 F", "F' L U2 L' R' U R F", "F' L U2 R' U L' R F", "F' R' U' L R U2 L' F", "R' U' R F' L U2 L' F", "L U2 L' B R U R' B'", "F' R' U R2 B2 R' B2 F"], ["F2 U B' U' B F2", "L R2 U' L' U R2", "F' U2 L U2 L' F"], ["F' L' U L2 U' F' L' F2", "F' L2 U L' U' F' L2 F2", "B' U2 R U' R B' R2 B2", "B2 D' R' D R2 B R2 B", "R U' B2 R B R' B R'", "R U' L R' F' U2 F L'", "L2 F2 L' U' F' U F2 L'", "B2 D' R D B2 F' U F", "F' U B L' B' L U2 F", "R U' R' F' L U2 L' F"], ["L F' U' F' L' F2", "L2 F' L' F' L' F2"], ["L' U L2 F' U2 F L'", "L' U L2 F' U2 L' F"]],
[["U B' R' B R' B' R B", "R2 B U2 B' U2 R' U' R'", "U' R' U R U2 R U' R'", "R2 F R2 F' U2 R U' R'", "R D R' U2 R D' U' R'", "U R' U2 R U' R U R'", "U' R' U' R U2 R U R'", "R' U F' U' F R2 U R'", "R D B U' B' D' U R'", "U' R' U R U' R U2 R'", "R2 F R2 F' U' R U2 R'", "R D R' U' R D' U2 R'", "U R' U' R' U R' U' R2", "B U' B' R2 U R U' R2", "R2 F' U' F U2 R U' R2", "U2 F' D2 L' U' L D2 F", "B U L F' U' F L' B'"], ["B' R2 B U R2 U' R2", "B U2 B' U' F' U' F", "U2 B U B' F' U' F", "U2 B U' B' F' U F", "F' U2 B U' B' U F", "U2 B U F' U' B' F"], ["R' U' R' U R' U' R2", "B' R' B R' B' R B", "R' U2 R U' R U R'"], ["U B U B' F' U' F", "U B U' B' F' U F", "F' U B U' B' U F", "U B U F' U' B' F"], ["F' D2 L' U' L D2 F"], ["B U F' U' B' F", "B U B' F' U' F", "B U' B' F' U F"], ["R' U R U2 R U' R'", "R' U' R U2 R U R'", "R' U R U' R U2 R'"], ["F R F2 U' F2 R' F'", "U' B U B' F' U' F", "U' B U' B' F' U F", "F' U' B U' B' U F", "U' B U F' U' B' F"], ["B2 R2 B2 U R2 U' R2", "B2 U B2 U' R2 B2 R2", "B' R2 B D B2 D' R2", "F' D B' U2 B D' F", "F2 D L2 D' F2 U B2", "B R B2 R B' R2 B'", "B R B' R B2 R2 B'"], ["R2 U2 B' U2 R' U' R' B", "B' U R' B R' B' R B", "R2 U2 B' U2 B R' U' R'", "R2 U2 B' U2 R' B U' R'", "R' U B' U' B R2 U2 R'", "R2 U2 B' U2 R' U' B R'", "B U B' L F' U' F L'", "B' R2 U R' B R2 U' R2", "B U L' B' L F' U' F"], ["R' B' L2 F R2 F' L2 B R", "R2 B U2 B' R' U' R' U' R2", "R2 U2 B' R' B U' R' U' R2", "R' B' R' B R' U R' U' R2", "R' U' R' U R' U2 R2 U R2", "B' R2 B U R2 U2 R2 U R2", "R' U' R' U R' U R2 U2 R2", "B' R2 B U R2 U R2 U2 R2", "R2 U' R' U' F R' F' U2 R2", "F2 U2 R2 U' R2 U' F2 U2 R2", 'B2 U2 R2 U R2 U B2 U2 R2', "R2 U' R' U' R' F' U2 F R2", "D' R U' R' D B' R2 B R2", "B L U2 L' U2 B2 R2 B R2", "B U' R U R' B2 R2 B R2", "B U2 R U2 R' B2 R2 B R2", "B' D' B U B' D R2 B R2", "R2 U2 R2 U R2 U F R2 F'", "R2 U R2 U2 R2 U F R2 F'", "R2 F R2 D F' U F D' F'", "R2 U2 R2 U R' U R' U' R'", "R2 U R2 U2 R' U R' U' R'", "R' U R' U2 R2 U2 R' U' R'", "R' U R2 U2 R' U2 R U' R'", "R' U' R' U2 R2 U2 R' U R'", "R' U2 R U' R' U2 R2 U R'", "R' U' R U2 R' U2 R2 U R'", "R' U F' U' F U2 R2 U R'", "R' U B' R' B U2 R2 U R'", "R' U R2 U2 F R' F' U R'", "R' U R2 U2 B U' B' U R'", "R' U R2 U2 R' U' R U2 R'", "R2 U' R' U R' F R' F' R'", "B U' B' R U' F R F' R'", "R F L2 B' R2 B L2 F' R'", "R2 F R2 F' D R' U' R D'", "R2 F R2 F2 R' U R U' F", "R' B' R B U' R F' U' F", "B R U2 R' U B' F' U' F", "B R U2 R' U' B' F' U F", "R2 F R2 F2 R' U2 R U2 F", "B U B' F' U' R' U2 R F", "B U' B' F' U R' U2 R F", "B F' U' B' U R' U2 R F", "R2 F R2 F2 U2 L' U2 L F", "F2 D2 B2 D' L2 D' F2 U B2", "B2 U2 F2 D' L2 D F2 U B2", "R2 U2 B2 U' R2 U' R2 U2 B2", "B2 U2 R2 D B2 D' R2 U2 B2", "F2 U B2 D' L2 D' F2 D2 B2", "B2 D2 L2 D F2 D' L2 D2 B2", 'R2 U2 F2 U R2 U R2 U2 F2', "F2 U2 R2 D' F2 D R2 U2 F2", "F2 U B2 D L2 D' B2 U2 F2", "F2 D2 L2 D' B2 D L2 D2 F2", "B R U2 R' U F' U' B' F"], ["F R2 F' R' U' R'", "B U2 B' R U' R'", "B' R B2 U B' R'", "B U2 R U' R' B'", "B U' R U2 R' B'", "R' U' R' B' R2 B", "F' R' U2 R U' F", "F' R' U' R U2 F", "R' U' R F' U2 F", "R' F' U F2 R F'"], ["B F' U' B' U F"], ["R' U B' R' B R2 U R'", "B' R B U F R F' R'"], ["R2 D' F2 D F R2 F'", "B F U' B' F2 U F", "F2 U B2 D' L2 D B2", "B L U F2 L' B' F2", "B U L F2 L' B' F2", "L' R' U' L R2 U' R'", "R2 F2 U' F2 U R2 F2", "R2 D' F2 D F2 R2 F2", "B U B' L F2 L' F2"], ["D' F2 U' F' D R' U' R'", "D2 F U F' D2 R U2 R'", "B' R B F R U' F' R'", "B U B' L' F' U' F L", "B U B' R' D' F' D R", "R' F' U F D' F' D R", "R' B' R B D' F' D R", "F R U' F' R' F R' F'", "F' R' U F' U' R U F", "D2 L' U2 L D2 F' U F", "F' R2 F' R F2 U R F", "B U R' D' F' D R B'", "B U L' F' U' F L B'", "D' R U2 F U' F' R' D", "D' L' F' U' F U2 L D"]],
[["R' U2 R2 U' R'", "R' U' R2 U2 R'"], ['B2 U2 R2 B R2 U2 B2', 'F2 D2 L2 B L2 D2 F2'], ["U' R' U2 R2 U' R'", "U' R' U' R2 U2 R'"], ["B' R D' R D B R2", "B' R2 D B' D' B2 R2", "R' U' R U2 F' U' F", "B' R B2 U' B' U R'"], ["U2 R' U2 R2 U' R'", "U2 R' U' R2 U2 R'"], ["R' U F' U' F2 R F'", "R2 U' F' U F2 R2 F'", "R2 U2 F' U2 F2 R2 F'", "R2 F2 D' F' D R2 F'", 'F R2 F2 U2 F U2 R2', "B' R2 B2 U2 B' U2 R2", "R' U2 R U' F' U' F", 'B2 D2 L2 F L2 D2 B2', 'R2 U2 B U2 B2 R2 B', 'F2 U2 R2 F R2 U2 F2'], ["U R' U2 R2 U' R'", "U R' U' R2 U2 R'"], ["B' R2 B2 U B' U' R2", "B U' B' U F' U' F", "B U F' U B' U F"], ["R2 F D2 B D2 R2 F'", "R2 F D2 B2 D2 F' R2", "R2 B R2 B2 R2 B' R2"], ["B2 U2 B R U R' U B2", "R2 B' R U2 R2 U' R' B", "R B' R2 U2 R2 U' R' B", "R2 B' R U' R2 U2 R' B", "R B' R2 U' R2 U2 R' B", "R2 B' R U2 R2 B U' R'", "R B' R2 U2 R2 B U' R'", "B2 U2 B R U2 B2 U' R'", "B' R2 U B U' R' U R'", "B' R2 U2 B U2 R' U R'", "R' B' U' R' B R2 U2 R'", "B U2 B2 U' R B U2 R'", "R D B' U' B' D' U2 R'", "R D B D' F R2 F' R'", "R2 B' R U2 R2 U' B R'", "R B' R2 U2 R2 U' B R'", "R2 B' R U' R2 U2 B R'", "R B' R2 U' R2 U2 B R'", "B' U' B2 R B2 U2 R' B'"], ["R2 F L2 B' R2 B L2 F'", "B' L2 F R2 F' L2 B R2"], ["R' U R F' U2 F2 R' F' R", "R' U R F' U2 F2 R2 F' R2", "R' U R B U2 B2 R2 B R2", "R2 F R2 F2 U2 F R U R'", "B2 L' B' L U' B' R U R'", "R B' R' B2 U2 B' R U R'", "R2 B' R2 B2 U2 B' R U R'", "B U' B' U2 R F R' F' R'", "R' B' R' B R U2 F' U' F", "B2 L F2 U' F' U F2 L' B2", "R' U R F' U' L F' L' F2", "F2 L' B2 U B' U' B2 L F2"], ["R U' R2 U2 R2 U' R'", "R U' R2 U' R2 U2 R'", "F2 U R' U R U2 F2"], ["B' R2 B R' U R'"], ["F' R' U2 F2 R F"], ["F U2 F' R' U2 R2 U' R'", "F U2 R' F' U2 R2 U' R'", "B' F2 R2 B F2 R' U R'", "F U2 F' R' U' R2 U2 R'", "F U2 R' F' U' R2 U2 R'", "F U2 R' U' F' R2 U2 R'", "R' U2 R2 F R' U' F' R'", "R' U2 R2 U' F U' F' R'", "R' U' R2 U2 F U' F' R'", "R2 F2 R2 F' D R D' F'"]],
[["B U B' R U R'"], ["U B F' U B' F", "F' U B U B' F"], ["B' R2 B R F R F'", "R' U' R2 B' R B R2", "B' R2 D' R D R B", "U2 B U R U R' B'", "U' B U B' R U R'"], ["B F' U B' F"], ["U B U R U R' B'", "R' U R U R U R'", "U2 B U B' R U R'"], ["U' B F' U B' F"], ["B U R U R' B'"], ["U2 B F' U B' F", "F' U2 B U B' F", 'B U B2 R2 B R2'], ["B U R2 B R2 B'", "B R' B R B' R"], ["B U B2 R U R' B", "R' U' R2 B' U R' B", "B U B2 R U B R'", "R' U' R2 B' U B R'"], ["R F2 U' R F2 R' U F2", "R F2 U2 R F2 R' U2 F2", "B2 U R' B2 R U' B2 R", "B2 U2 R' B2 R U2 B2 R"], ["R' B' R B R2 U R'", "R' U R2 F R F' R'"], ["F' U' B U B' F"], ["F' R' U2 R F R U R'", "B' D' R2 D B R U R'", "R' U R2 U F R F' R'", "R' U2 R2 U2 R' F' U' F", "F R' F' U R F' U F", "B' D' R D B F' U F", "F' U2 R B' R' B U F"], ["L R2 F2 L' R2 F2"], ["R' D' F2 U F D R", "F' R' F' U R U F"]],
[["F' U F R' F R F'", "L' B2 D' R D B2 L", "F' U F U R U' R'", "F' U' F U R U R'", "R' F R F' R U R'", "F' U F U2 R U2 R'", "R U2 B U B' U2 R'"], ["F U2 L F2 L' U2 F'", "F' U2 L' U2 L U2 F", "B' R2 U' R2 U R2 B", "B U B2 D' R2 D B", "B' D' R2 D B2 U' B'", "B L2 D F2 D' L2 B'"], ["F U2 L F' L' U2 F'", "L' B2 D' R' D B2 L", "R' U2 B' R' B U2 R", "B' R2 U' R' U R2 B"], ["L F2 U F' U' F2 L'", "R U2 R' U2 F' U' F", "R U2 B U' B' U2 R'"], ["F' U2 L' U L U2 F", "B' R2 U' R U R2 B", "F' U2 F U2 R U R'"], ["L F2 U F U' F2 L'", "F U2 L F L' U2 F'", "R' U2 B' R B U2 R", "B L2 D F D' L2 B'"], ["L F2 U F2 U' F2 L'", "L' U' L2 D F2 D' L'", "L D F2 D' L2 U L", "L' B2 D' R2 D B2 L", "R' U2 B' R2 B U2 R", "R U2 B U2 B' U2 R'"], ["R U' R' F R' F' R", "R U R' U' F' U' F", "F R' F' R F' U' F", "R U' R' U' F' U F", "R U' R' U2 F' U2 F", "F' U2 L' U' L U2 F", "B L2 D F' D' L2 B'"], ["L2 D F2 D' L2", "B2 D' R2 D B2"], ["R2 B' R' U B R'", "F2 L F U' L' F"], ['L2 D2 R2 D2 L2', "R' B' R2 B R", "R2 U' R2 U R2", 'R2 U2 R2 U2 R2'], ["F' R' U R F", "B' D' R D B"], [], ["R U' R U2 F R2 F' U2 R2", "R2 U2 F R2 F' U2 R' U R'", "F2 U2 R' F2 R U2 F U' F", "F' U F' U2 R' F2 R U2 F2"], ["F2 U F2 U' F2", 'F2 U2 F2 U2 F2', "F L F2 L' F'", 'B2 D2 F2 D2 B2'], ["R F U' F' R'", "L D F' D' L'"]],
[["R B U2 B' U R'"], ["R U' R' F' U' F", "F2 L F L' U2 F"], ["F' U2 F R U2 R'", "R U2 B' R B R2", "R U' R2 F R F'"], ["U' R U' R' F' U' F", "U2 R' F R F2 U' F", "U' F2 L F L' U2 F", "U2 F' U L' U2 L F"], ["U' R U' R2 F R F'", "U' R U2 B' R B R2", "U2 R B U2 B' U R'", "U' F' U2 F R U2 R'"], ["F' U' F U F' U' F", "U2 R U' R' F' U' F", "U R' F R F2 U' F", "U2 F2 L F L' U2 F", "U F' U L' U2 L F"], ["U2 R U' R2 F R F'", "U2 R U2 B' R B R2", "R U B U B' U R'", "U R B U2 B' U R'", "R B' R B R' U2 R'", "U2 F' U2 F R U2 R'", "L U' R U L' U2 R'"], ["R' F R F2 U' F", "F' U L' U2 L F"], ["F' U F' D' L2 D F2"], ["R U2 B' U B R'", "F' L U L' U2 F"], ["R2 B U B' R2 U' R2", "F2 D' F' D F' U' R2", "F' U B U B' U F", "B U' F' U2 B' U F"], ["R' U2 F R F2 U' F"], ["R F' L F' L' F2 R U2 R2", "R U2 R U2 F R F' U2 R2", "R U2 R B2 L' B' L B' R2", "R2 U2 R' U' R U' R' U2 R'", "R2 U B U' B' R2 F' U' F", "F2 U2 R' F R U2 F U2 F", "F2 L' B L' B' L2 F U2 F", "F2 U2 F R2 B' R' B R' F", "R U' R' F2 L' U' L U F2", "F' U2 F' U' F U' F' U2 F2"], ["F' L' U2 L F R U R'", "F' U F R B U2 B' R'", "R U' R U B U' B' R2", 'R U2 B U B2 R B R2', "F2 L' U' L U F U' F", 'F2 L F L2 U L U2 F'], ["R U L' U2 L R'", "F' U R' F2 R F"], ["R U' R2 D' F' D R"]],
[["F R' F' R2 U R'", "R U' B U2 B' R'"], ["U2 F' U F2 R' F' R", "F' U' L' U' L U' F", "U' F' L' U2 L U' F", "U2 R U2 R' F' U2 F", "F' L F' L' F U2 F", "B' U F' U' B U2 F", "U2 F' U2 L F' L' F2"], ["R U R' U' R U R'", "U2 F' U F R U R'", "U' F R' F' R2 U R'", "U2 R2 B' R' B U2 R'", "U' R U' B U2 B' R'"], ["U F' U F2 R' F' R", "U2 F' L' U2 L U' F", "U R U2 R' F' U2 F", "U F' U2 L F' L' F2"], ["U F' U F R U R'", "U2 F R' F' R2 U R'", "U R2 B' R' B U2 R'", "U2 R U' B U2 B' R'"], ["F' U2 L F' L' F2", "F' U F2 R' F' R", "R U2 R' F' U2 F"], ["F' U F R U R'", "R2 B' R' B U2 R'"], ["F' L' U2 L U' F"], ["R U' R D B2 D' R2"], ["R B' U' B U2 R'", "F' U2 L U' L' F"], ["R U' F R2 F' R'", "F' U' B U2 B' F"], ["F' U F2 D R D' F'"], ["F' U F R2 B U B' U' R2", "R U2 R U R' U R U2 R2", "F2 U' L' U L F2 R U R'", "R2 U2 F R' F' U2 R' U2 R'", "R2 B L' B L B2 R' U2 R'", "R2 U2 R' F2 L F L' F R'", "F2 U2 F U F' U F U2 F", "F' U2 F' U2 R' F' R U2 F2", "F' R B' R B R2 F' U2 F2", "F' U2 F' L2 B L B' L F2"], ["F' U2 L' U' L2 F' L' F2", "F' U F' U' L' U L F2", "R2 B U B' U' R' U R'", "R2 B' R' B2 U' B' U2 R'", "R B U2 B' R' F' U' F", "R U' R' F' L' U2 L F"], ["R U' L' U' L U' R'", "L' U R U2 L U' R'", "R2 D R D' R U F2", "F2 L' U' L F2 U F2"], ["F U2 R' F' R2 U R'"]],
[["R' F' R2 U R2 F R", "U L' U' R U L R'", "U L' U L R U' R'", "R U L' U L U' R'", "U L' U' L R U R'"], ["F U F' U2 F' U' F", "F U' F' U2 F' U F", "F U' F' U F' U2 F"], ["L' U L R U' R'", "L' U' L R U R'", "L' U' R U L R'"], ["R D2 B U B' D2 R'"], ["U' L' U' R U L R'", "U' L' U L R U' R'", "R U' L' U L U' R'", "U' L' U' L R U R'"], ["L F L' F L F' L'", "F U2 F' U F' U' F", "F U F U' F U F2"], ["U2 L' U' R U L R'", "U2 L' U L R U' R'", "R U2 L' U L U' R'", "L' U2 L U R U R'", "U2 L' U' L R U R'", "L F2 L' U' F2 U F2"], ["L' U L F2 U' F' U F2", "F2 R U R' U2 F' U F2", "U' F U F U' F U F2", "U2 R D2 B U B' D2 R'", "U' L F L' F L F' L'", "L' U' B' R U R' B L", "U' F U2 F' U F' U' F", "U F U F' U2 F' U' F", "F U' R U R' F2 U' F", "F' D' L' U L D U' F", "F2 R' F2 R U2 F' U F", "U F U' F' U2 F' U F", "F2 L' U2 L U2 F U F", "F' D' F U2 F' D U F", "F2 R' F2 R U F' U2 F", "U F U' F' U F' U2 F", "F' D' F U F' D U2 F"], ["R2 D' B2 D R2 U' L2", "L' F' L2 F' L F2 L", "L' F' L F' L2 F2 L", "R D' L U2 L' D R'", "L2 F2 L2 U' F2 U F2", "L2 U' L2 U F2 L2 F2", "L F2 L' D' L2 D F2"], ["L F2 U' F L' F2 U F2", "L' U' L B' R U R' B", "L' U' B L B' R U R'", "L U' F L' F L F' L'", "F2 U2 L U2 F U F L'", "F2 U2 L U2 L' F U F", "F2 U2 L U2 F L' U F", "F U' L U L' F2 U2 F", "F2 U2 L U2 F U L' F"], ["F2 R2 U R2 U' F2 R2", "F2 D R2 D' R2 F2 R2", "L' U' L B' R2 B R2", "R2 U' L2 D B2 D' L2", "F2 D R2 D' R' F2 R", "B F U B' F2 U F", "L' B' U' R2 B L R2", "L' U' B' R2 B L R2", "L' R' U L R2 U' R'"], ["D2 B U2 B' D2 R U' R'", "R F U' R U F' U' R'", "R F2 R F' R2 U' F' R'", "L' U' F D R D' F' L", "L' U' B R U R' B' L", "R' F' U R F R' F R", "F R U' R' D R D' F'", "L' U' L F D R D' F'", "F L F' L' D R D' F'", "D R2 U R D' F U F", "D2 R' U' R D2 F' U2 F", "L F' L' R' F' U R F", "D F' U2 R' U R F D'", "D B R U R' U2 B' D'", "L' U' L B R U R' B'"], ["L' R U L U' R'"], ["F U' L F L' F2 U' F", "L F' L' U' R' F' R F"], ["F2 U2 F2 U' F2 U' R' F2 R", "F2 U' F2 U2 F2 U' R' F2 R", "F2 R' F2 D' R U' R' D R", "F2 U2 R2 U' F2 U' F2 U2 R2", "R2 U2 F2 D R2 D' F2 U2 R2", "R2 U' L2 D' B2 D L2 U2 R2", "R2 D2 B2 D L2 D' B2 D2 R2", "L2 U2 R2 D B2 D' R2 U' L2", "R2 D2 L2 D B2 D R2 U' L2", 'F2 U2 L2 U F2 U F2 U2 L2', "L2 U2 F2 D' L2 D F2 U2 L2", "R2 U' L2 D B2 D R2 D2 L2", "L2 D2 B2 D' R2 D B2 D2 L2", "F L B2 R' F2 R B2 L' F'", "L' F' U2 F U L R U' R'", "F L F' L' U F' R U R'", "L' F' U2 F U' L R U R'", "F2 R' F2 R2 F U' F' U R'", "F2 R' F2 R2 F U2 F' U2 R'", "L' U L R U' F U2 F' R'", "L' R U L U' F U2 F' R'", "L' U' L R U F U2 F' R'", "F2 R' F2 R2 U2 B U2 B' R'", "F U' F2 U2 R' F R U' F", "F U F U2 F2 U2 F U' F", "F U' R U R' U2 F2 U' F", "F U2 F' U F U2 F2 U' F", "F U F' U2 F U2 F2 U' F", "F U' L F L' U2 F2 U' F", "F U' F2 U2 L' U L U' F", "F U' F2 U2 F U2 F' U F", "F2 U2 F2 U' F U' F U F", "F2 U' F2 U2 F U' F U F", "F U' F U2 F2 U2 F U F", "F U' F2 U2 F U F' U2 F", "L' U L F' U R' F' R F", "F2 U F U' F R' F R F", "F' R' B2 L F2 L' B2 R F", "F U F U' F U2 F2 U' F2", "L F2 L' U' F2 U2 F2 U' F2", "F L F L' F U' F U F2", "F2 L' U2 L F U F U F2", "F2 U2 L F L' U F U F2", "F2 U F U R' F R U2 F2", 'R2 U2 F2 U F2 U R2 U2 F2', "F U F U' F U' F2 U2 F2", "L F2 L' U' F2 U' F2 U2 F2", "L2 U2 F2 U' F2 U' L2 U2 F2", "F2 U F U F R U2 R' F2", "D F' U F D' L F2 L' F2", "L' B' U2 B U2 L2 F2 L' F2", "L' U F' U' F L2 F2 L' F2", "L' U2 F' U2 F L2 F2 L' F2", "L D L' U' L D' F2 L' F2", "L' F' U2 F U' R U L R'", "F2 R' F2 R D' F U F' D"], ["R F U2 F' U R'", "F U F' R U2 R'", "R F U F' U2 R'", "L' U2 F' U F L", "L' U F' U2 F L", "F R U' R2 F' R", "L' U2 L F' U F", "R' F2 R F U F", "L F' L2 U' L F", "F U F L F2 L'"]],
[["U2 L' R U' L R'", "R U2 L' U' L R'", "L' U' L2 F2 L' F2"], ["L' U' F' U' F L"], ["U L' R U' L R'"], ["U' L' U' F' U' F L", "F U' F' U' F' U' F", "U2 L' U' L F' U' F"], ["L' R U' L R'"], ["L F2 D F' D' F' L'", "U2 L' U' F' U' F L", "L F2 L' F' R' F' R", "U L' U' L F' U' F", "F U F2 L F' L' F2"], ["U' L' R U' L R'", "R U' L' U' L R'"], ["L' U' L F' U' F"], ["L' U' F2 L' F2 L", "L' F L' F' L F'"], ["L' U' L2 F' U' F L'", "F U F2 L U' F L'", "L' U' L2 F' U' L' F", "F U F2 L U' L' F"], ["B' F2 R2 B F2 R2"], ["F D R2 U' R' D' F'", "R F R U' F' U' R'"], ["R U L' U' L R'"], ["R' F R U' F' R U' R'", "L D F' D' L' R U' R'", "R U2 F' L F L' U' R'", "F U2 F2 U2 F R U R'", "R F U2 F' R' F' U' F", "L D F2 D' L' F' U' F", "F U' F2 U' R' F' R F"], ["L2 U' F L2 F' U L2 F'", "L2 U2 F L2 F' U2 L2 F'", "F' R2 U F' R2 F U' R2", "F' R2 U2 F' R2 F U2 R2"], ["F L F' L' F2 U' F", "F U' F2 R' F' R F"]],
[["L' U' R U' L U' R'", "L' U L U' R U R'", "L F2 L2 U' L U F2"], ["U' F U2 F2 U F", "U' F U F2 U2 F"], ["F2 U2 L' U2 L2 F2 L'", "R2 U2 F2 R' F2 U2 R2", "L2 D2 B2 R' B2 D2 L2", "F U' R U R2 F' R", "F2 U R U' R2 F2 R", 'F2 U2 R U2 R2 F2 R', "F2 R2 D R D' F2 R", "F U2 F' U R U R'", "R' F2 R2 U2 R' U2 F2", 'L F2 L2 U2 L U2 F2'], ['U2 F U2 F2 U F', 'U2 F U F2 U2 F'], ["L F' L2 U L U' F", "F U F' U2 R U R'", "L F' D F' D' L' F2", "L F2 D' L D L2 F2"], ['U F U2 F2 U F', 'U F U F2 U2 F'], ["R2 D2 B2 L' B2 D2 R2", "L2 U2 F2 L' F2 U2 L2"], ['F U2 F2 U F', 'F U F2 U2 F'], ["F2 R' D2 L' D2 F2 R", "F2 R' D2 L2 D2 R F2", "F2 L' F2 L2 F2 L F2"], ["F2 L F' U2 F2 U F L'", "F' L F2 U2 F2 U F L'", "F2 L F' U F2 U2 F L'", "F' L F2 U F2 U2 F L'", "L U L2 F' L2 U2 F L", "L2 U2 L' F' U' F U' L2", "L F2 U' L' U F U' F", "L F2 U2 L' U2 F U' F", "F2 L F' U2 F2 L' U F", "F' L F2 U2 F2 L' U F", "L2 U2 L' F' U2 L2 U F", "F L U F L' F2 U2 F", "L' U2 L2 U F' L' U2 F", "F' D' L U L D U2 F", "F' D' L' D R' F2 R F", "F2 L F' U2 F2 U L' F", "F' L F2 U2 F2 U L' F", "F2 L F' U F2 U2 L' F", "F' L F2 U F2 U2 L' F"], ["R F U2 R2 F' R'"], ["F2 R2 F2 R D' F' D R", "L R2 F2 L' R2 F U' F", "R' U2 F R U2 F2 U F", "R' U2 R F U2 F2 U F", "R' U2 F R U F2 U2 F", "R' U2 R F U F2 U2 F", "R' U2 F U R F2 U2 F", "F U2 F2 U R' U R F", "F U F2 U2 R' U R F", "F U2 F2 R' F U R F"], ["R2 U' F U' F' U2 R2", "F' U F2 U2 F2 U F", "F' U F2 U F2 U2 F"], ["L F2 L' F U' F"], ["L B2 R' F2 R B2 L' F2", "F2 R' B2 L F2 L' B2 R"], ["R2 B L2 U' L U L2 B' R2", "F U' F' R U B' R B R2", "L2 B' R2 U R U' R2 B L2", "F U' F' R U2 R2 F R F'", "F L F L' F' U2 R U R'", "F2 R' F2 R2 U2 R' F' U' F", "L2 B L B' U L F' U' F", "F' L F L2 U2 L F' U' F", "F2 L F2 L2 U2 L F' U' F", "L' U L U2 F' R' F R F", "F U' F' R U2 R2 F2 R F2", "F U' F' L' U2 L2 F2 L' F2"]]]

FULL_OLL = ('', "R2 D' R U2 R' D R U2 R", "B2 D' B U2 B' D B U2 B", "L' B L F' L' B' L F", "L F R' F' L' F R F'", "F U2 F' U' F U' F'", '', '', "F' L F R' F' L' F R", "R B L' B' R' B L B'", "L U L' U L U2 L'", "R U2 R2 U' R2 U' R2 U2 R", "L2 D' L U2 L' D L U2 L", "R U2 R' U' R U R' U' R U' R'", '', '', "B' R B L' B' R' B L", "L U2 L' U' L U' L'", "F R B' R' F' R B R'", 
            "F U2 F2 U' F2 U' F2 U2 F", "R U2 R' U' R U' R'", "B U2 B' U' B U' B'", '', '', '', '', '', '', '', '', '', '', "B L F' L' B' L F L'", '', "B U B' U B U2 B'", '', "R' F R B' R' F' R B", '', '', '', "R U R' U R U2 R'", '', "F U F' U F U2 F'", '', "B U2 B2 U' B2 U' B2 U2 B", '', '', '', "F2 D' F U2 F' D F U2 F", '', "F U2 F' U' F U F' U' F U' F'", '', "L U2 L2 U' L2 U' L2 U2 L", 
            '', '', '', '', '', '', '', '', '', '', '', "R B L' B' R' L U L U' L'", "L' U' B' U B L", "B U L U' L' B'", "B L' B' L U L U' L'", "R U B' U' R' U R B R'", "B L2 F' L' F L' B'", '', '', "L U L' U L U' L' U' L' B L B'", "L U L' U' L U' L' B' U' B L U L'", "R B L' B L B2 R'", "L' B L2 F' L2 B' L2 F L'", "L U L' U L U2 L' B L U L' U' B'", "R B2 L' B' L B L' B' L B' R'", '', '', 
            "B' U' B U' B' U B U B L' B' L", "F' L' B L' B' L2 F", "B L' B L2 U' L' U' L U L' B2", "B L' B2 R B2 L B2 R' B", "B F2 L' F L' F' L2 F L' F B'", "L U L' U' L' B L2 U L' U' B'", '', '', '', '', '', '', '', '', '', '', "F' U' L U F U' F' L' F", '', "L' B2 R B R' B L", '', "R U2 R2 F R F' R U2 R'", '', '', '', "R B L' B L' D L D' L B2 R'", '', "B U B' U B' R B R' B U2 B'", '', 
            "L' B' U' B U B' U' B U L", '', '', '', "B' U' B U' B' U2 B R B U B' U' R'", '', "F' L2 B L B' L' B L B' L F", '', "B L U L' U' L U L' U' B'", '', '', '', '', '', '', '', '', '', '', '', "R U R' U' L R' F R F' L'", "B' U' B' R B R' U B", "F R U R' U' F'", "R' F R U R' U' F' U R", "R U R' U' R' F R F'", "R' F R U R' F' R F U' F'", '', '', "R B' R' U' R U B U' R'", 
            "L U L' U' L' B L B'", "R B' R' U' R B R' B' U B", "B U L U' L' U L U' L' B'", "F' U' F' L F L' U F", "F U2 F2 U' F U' F' U2 L F L'", '', '', "L F' L' U' L U F U' L'", "R B R' L U L' U' R B' R'", "L U L' U' F' L' B L B' F", "B U B' U B U' L U' L' B'", "L F L' R U R' U' L F' L'", "L' B L U L' B' L B U' B'", '', '', '', '', '', '', '', '', '', '', "R U R' U' B' R' F R F' B", 
            '', "L' B' L R' U' R U L' B L", '', "L' B L U L' U' B' U L", '', '', '', "L F' L' U' L F L' F' U F", '', "R' F' R L' U' L U R' F R", '', "F U F' U F U' R U' R' F'", '', '', '', "B L U L' U' B'", '', "L' B' L U' R' U R U' R' U R L' B L", '', "F U R U' R' U R U' R' F'", '', '', '', '', '', '', '', '', '', '', '', "B L F' L' B' F U F U' F'", "L' U' L U' L' U2 L B L U L' U' B'", 
            "F' U' L' U L F", "L' U' L U' L' U L U L F' L' F", "R' U' F U R U' R' F' R", "R' F' L F' L' F2 R", '', '', "L F' L' F U F U' F'", "L F' L F2 U' F' U' F U F' L2", "F' L2 B L B' L F", "L F' L2 B L2 F L2 B' L", "L U F U' F' L'", "R' F2 L F L' F' L F L' F R", '', '', "B U2 B2 R B R' B U2 B'", "F U F' U' F' L F2 U F' U' L'", "B U L' U' B' U B L B'", "L F U F' U' F U F' U' L'", 
            "L F2 R' F' R F' L'", "L R2 F' R F' R' F2 R F' R L'", '', '', '', '', '', '', '', '', '', '', "F U F' U' F U' F' L' U' L F U F'", '', "B L F' L F' D F D' F L2 B'", '', "F U F' U F U' F' U' F' L F L'", '', '', '', "L U L' U L' B L B' L U2 L'", '', "B L F' L F L2 B'", '', "F' L F2 R' F2 L' F2 R F'", '', '', '', "F U F' U F U2 F' L F U F' U' L'", '', "B L2 F' L' F L F' L' F L' B'", 
            '', "F' L' U' L U L' U' L U F", '', '', '', '', '', '', '', '', '', '', '', "F R B' R' F' B U B U' B'", "R U B U' B' R'", "B U B' U B U2 B' R B U B' U' R'", "B U B' U B U' B' U' B' R B R'", "R B' R B2 U' B' U' B U B' R2", "R L2 B' L B' L' B2 L B' L R'", '', '', "F U2 F2 L F L' F U2 F'", "L' U' B U L U' L' B' L", "R U R' U R' F R F' R U2 R'", "B' R' U' R U R' U' R U B", 
            "R' U' R U' R' U2 R F R U R' U' F'", "L' B2 R B R' B' R B R' B L", '', '', "R B' R' B U B U' B'", "R B2 L' B' L B' R'", "B U B' U' B U' B' R' U' R B U B'", "B' R B2 L' B2 R' B2 L B'", "B U B' U' B' R B2 U B' U' R'", "L' B' R B' R' B2 L", '', '', '', '', '', '', '', '', '', '', "F U R' U' F' U F R F'", '', "F R B' R B R2 F'", '', "R' U' R U' R' U R U R B' R' B", '', 
            '', '', "B' R2 F R F' R B", '', "F R B' R B' D B D' B R2 F'", '', "R B U B' U' B U B' U' R'", '', '', '', "B' U' R' U R B", '', "F R2 B' R' B R B' R' B R' F'", '', "R B' R2 F R2 B R2 F' R", '', '', '', '', '', '', '', '', '', '', '', "F U F' U' B F' L F L' B'", "L F U F' U' L'", "L' U' L' B L B' U L", "F R' F' U' F U R U' F'", "B U B' U' L' B' R B R' L", "B L B' F U F' U' B L' B'", 
            '', '', "B' R B U B' U' R' U B", "F U F' U' R' F' L F L' R", "F' L' F B' U' B U F' L F", "L U L' U L U' F U' F' L'", "R B U B' U' R'", "B' R' B U' F' U F U' F' U F B' R B", '', '', "F' L F U F' U' L' U F", "F' L F U F' L' F L U' L'", "B U B' U' B' R B R'", "R U B U' B' U B U' B' R'", "B' R B U B' R' B R U' R'", "F R F' B U B' U' F R' F'", '', '', '', '', '', '', '', 
            '', '', '', "F U F' U' F' L F L'", '', "F R' F' U' F R F' R' U R", '', "B L' B' U' B U L U' B'", '', '', '', "B' R' B F' U' F U B' R B", '', "B L' B' U' B L B' L' U L", '', "L U F U' F' U F U' F' L'", '', '', '', "R' U' R' F R F' U R", '', "R U2 R2 U' R U' R' U2 F R F'", '', "R U R' U R U' B U' B' R'", '', '', '', '', '', '', '', '', '', '', '', "L F R' F' L' R U R U' R'", 
            "R U R' U R U2 R' F R U R' U' F'", "F' U' F U' F' U2 F L F U F' U' L'", "L U2 L2 B L B' L U2 L'", "R U R' U' R U' R' F' U' F R U R'", "R U R' U' R' F R2 U R' U' F'", '', '', "F' U' F U' F' U F U F R' F' R", "L U F' U' L' U L F L'", "L F R' F R' D R D' R F2 L'", "F R U R' U' R U R' U' F'", "R' U' F' U F R", "L F2 R' F' R F R' F' R F' L'", '', '', "R U R' U R U' R' U' R' F R F'", 
            "F B2 R' B R' B' R2 B R' B F'", "B' U' R U B U' B' R' B", "R' F' U' F U F' U' F U R", "B' R' F R' F' R2 B", "F R2 B' R' B R' F'", '', '', '', '', '', '', '', '', '', '', "F R' F R2 U' R' U' R U R' F2", '', "F U F' U F' L F L' F U2 F'", '', "F R' F' R U R U' R'", '', '', '', "L F R' F R F2 L'", '', "R' F2 L F L' F R", '', "F R' F2 L F2 R F2 L' F", '', '', '', "F U R U' R' F'", 
            '', "B' R2 F R F' R' F R F' R B", '', "R' F R2 B' R2 F' R2 B R'", '', '', '', '', '', '', '', '', '', '', '', "B L F' L' B2 F2 R F R' F' R' B F'", "R B L' B L B2 R2 F' L F' L' F2 R", "B L F' L F L2 B2 R' F R' F' R2 B", "B L' B' L2 R' F L F' L' F' R L'", "B' F R F R F' R' B F2 L F L'", "L' R B' L U2 L' B' R B' R2 L", '', '', "L F' L' F2 B' R F R' F' R' B F'", 
            "F' B L B L B' L' F B2 R B R'", "R' L2 F L' F R U2 R' F R L'", "F R F' U2 F R2 B' R2 B R' F'", "L F R' F R F2 L2 B' R B' R' B2 L", "F U2 F2 L F L' U2 F' L F L'", '', '', "R B' R' B2 F' L B L' B' L' F B'", "B' F R' B U2 B' R' F R' F2 B", "L' R B R B R' B' L R2 F R F'", "L F L' U2 L F2 R' F2 R F' L'", "F' B L' F U2 F' L' B L' B2 F", "R' L F' R U2 R' F' L F' L2 R", '', '', 
            '', '', '', '', '', '', '', '', "R' L F L F L' F' R L2 B L B'", '', "F' B2 L B' L F U2 F' L F B'", '', "F R' F' R2 L' B R B' R' B' L R'", '', '', '', "L' R2 B R' B L U2 L' B L R'", '', "B' F2 R F' R B U2 B' R B F'", '', "R B R' U2 R B2 L' B2 L B' R'", '', '', '', "F R B' R B R2 F2 L' B L' B' L2 F", '', "R U2 R2 F R F' U2 R' F R F'", '', "B L B' U2 B L2 F' L2 F L' B'")

FULL_PLL = ('', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', "U' F U F' L' F U F' U' F' L F2 U' F'", '', "L2 D L D' L B2 R' U R B2", '', 
            "F2 U F U F' U' F' U' F' U F'", '', "F U' F U F U F U' F' U' F2", "L' U' B' L U L' U' L' B L2 U' L' U' L U L' U L", '', '', '', "U L U2 L' U2 L F' L' U' L U L F L2", "U' L R' F L2 R2 B L2 R2 F L R' D2 L2 R2", '', '', '', '', "R2 U R U R' U' R' U' R' U R'", '', '', "U R B' R F2 R' B R F2 R2", "U' F2 D R' U R' U' R D' F2 L' U L", '', "B2 U B U B' U' B' U' B' U B'", '', '', 
            "R U' R U R U R U' R' U' R2", '', "U B' U' B L2 D F' U F U' F D' L2", "L U L' U' L' B L2 U' L' U' L U L' B'", '', '', "L2 U L U L' U' L' U' L' U L'", '', '', '', '', 'L2 R2 D L2 R2 U2 L2 R2 D L2 R2', "U' F U F' L2 D' B U' B' U B' D L2", '', '', '', "U' B2 R2 B' L' B R2 B' L B'", "B U' B U B U B U' B' U' B2", '', "L U' L U L U L U' L' U' L2", '', "U B2 R B U B U' B' R' B U2 B' U2 B", 
            '', "U B2 D' R U' R U R' D B2 L U' L'", "U' F B' R F2 B2 L F2 B2 R F B' D2 F2 B2", '', '', '', '', '', '', '', '', '', '', '', '', "F2 L2 F' R' F L2 F' R F'", "U L2 B L U L U' L' B' L U2 L' U2 L", '', "U' R U R' F' R U R' U' R' F R2 U' R'", '', "B U B' R' B U B' U' B' R B2 U' B'", '', "B2 D L' U L' U' L D' B2 R' U R", "F U F' U' F' L F2 U' F' U' F U F' L'", '', '', '', 
            "F2 D F D' F L2 B' U B L2", "U R U R' U' R' F R2 U' R' U' R U R' F'", '', '', '', '', "U R2 D R D' R F2 L' U L F2", '', '', "U B L' B R2 B' L B R2 B2", "U L2 D' B U' B U B' D L2 F U' F'", '', "U2 R U2 R' U2 R B' R' U' R U R B R2", '', '', "B U B' R2 D' F U' F' U F' D R2", '', "U' R U R' F2 D' L U' L' U L' D F2", "F' U' L' F U F' U' F' L F2 U' F' U' F U F' U F", '', '', 
            "U2 F2 L F U F U' F' L' F U2 F' U2 F", '', '', '', '', "U2 L F' L B2 L' F L B2 L2", "U' R2 D B' U B' U' B D' R2 F' U F", '', '', '', "U' L2 B2 L' F' L B2 L' F L'", "U2 F' U' F R2 D B' U B U' B D' R2", '', "U2 F2 D' L U' L U L' D F2 R U' R'", '', "U F U2 F' U2 F R' F' U' F U F R F2", '', "U L' U' L F2 D R' U R U' R D' F2", "U R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R", 
            '', '', '', '', '', '', '', '', '', '', '', '', "F R' F L2 F' R F L2 F2", "F R' F R2 U' R' U' R U R' F' R U R' U' F'", '', "L' U L' U' F' L' F2 U' F' U F' L F L", '', "R' U' R B2 D L' U L U' L D' B2", '', "U2 L U L' B' L U L' U' L' B L2 U' L'", "U' R U' L U2 R' U L' R U' L U2 R' U L'", '', '', '', "B L' B L2 U' L' U' L U L' B' L U L' U' B'", "U' B U B' U' B' R B2 U' B' U' B U B' R'", 
            '', '', '', '', "R2 D' F U' F U F' D R2 B U' B'", '', '', "U R B' R' F R B R' F' R B R' F R B' R' F'", "U2 R B' R B2 U' B' U' B U B' R' B U B' U' R'", '', "U2 L2 D F' U F' U' F D' L2 B' U B", '', '', "U' B2 D B D' B R2 F' U F R2", '', "U2 L F' L F2 U' F' U' F U F' L' F U F' U' L'", "U' L' U R' U2 L U' R L' U R' U2 L U' R", '', '', "U2 L U L' B2 D' R U' R' U R' D B2", 
            '', '', '', '', "U2 R2 F2 R' B' R F2 R' B R'", "U2 F' U F' U' R' F' R2 U' R' U R' F R F", '', '', '', "U' B L' B' R B L B' R' B L B' R B L' B' R'", "B U2 B' U2 B L' B' U' B U B L B2", '', "R2 F R U R U' R' F' R U2 R' U2 R", '', "R' U R' U' B' R' B2 U' B' U B' R B R", '', "U2 B' U B' U' L' B' L2 U' L' U L' B L B", "U' B' U' R' B U B' U' B' R B2 U' B' U' B U B' U B", 
            '', '', '', '', '', '', '', '', '', '', '', '', "R B' R' F R B R' F' R B R' F R B' R' F'", "U R U2 R' U2 R B' R' U' R U R B R2", '', "U F2 L F U F U' F' L' F U2 F' U2 F", '', "U F' U F' U' R' F' R2 U' R' U R' F R F", '', "U' R' U R' U' B' R' B2 U' B' U B' R B R", "R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R", '', '', '', "U' B U B' R' B U B' U' B' R B2 U' B'", 
            "F U' B U2 F' U B' F U' B U2 F' U B'", '', '', '', '', "U L F' L F2 U' F' U' F U F' L' F U F' U' L'", '', '', "U L F' L B2 L' F L B2 L2", "U F' U' F R2 D B' U B U' B D' R2", '', "U B' U B' U' L' B' L2 U' L' U L' B L B", '', '', "U' F R' F R2 U' R' U' R U R' F' R U R' U' F'", '', "U' B2 D L' U L' U' L D' B2 R' U R", "R U R' U' R' F R2 U' R' U' R U R' F'", '', '', 
            "U R B' R B2 U' B' U' B U B' R' B U B' U' R'", '', '', '', '', "U2 B L' B' R B L B' R' B L B' R B L' B' R'", "U F2 D' L U' L U L' D F2 R U' R'", '', '', '', "U' F2 L2 F' R' F L2 F' R F'", "U' L' U L' U' F' L' F2 U' F' U F' L F L", '', "U' B L' B L2 U' L' U' L U L' B' L U L' U' B'", '', "R2 D R D' R F2 L' U L F2", '', "U' B U B' R2 D' F U' F' U F' D R2", "F' U B' U2 F U' B F' U B' U2 F U' B", 
            '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', "R2 F2 R' B' R F2 R' B R'", "U R U' R U R U R U' R' U' R2", '', "U B U' B U B U B U' B' U' B2", 
            '', "U2 R2 F R U R U' R' F' R U2 R' U2 R", '', "U2 R2 D' F U' F U F' D R2 B U' B'", "L R' F L2 R2 B L2 R2 F L R' D2 L2 R2", '', '', '', "U L U' L U L U L U' L' U' L2", "U B' U' R' B U B' U' B' R B2 U' B' U' B U B' U B", '', '', '', '', "L U L' B' L U L' U' L' B L2 U' L'", '', '', 'U', "U L2 U L U L' U' L' U' L' U L'", '', "U B2 D B D' B R2 F' U F R2", '', '', "L2 D F' U F' U' F D' L2 B' U B", 
            '', "U R2 U R U R' U' R' U' R' U R'", "F B' R F2 B2 L F2 B2 R F B' D2 F2 B2", '', '', "U2 B U2 B' U2 B L' B' U' B U B L B2", '', '', '', '', "U2 F R' F L2 F' R F L2 F2", "U F2 U F U F' U' F' U' F' U F'", '', '', '', 'U L2 R2 D L2 R2 U2 L2 R2 D L2 R2', "L U L' B2 D' R U' R' U R' D B2", '', "U2 R' U' R B2 D L' U L U' L D' B2", '', "U F U' F U F U F U' F' U' F2", '', "U B2 U B U B' U' B' U' B' U B'", 
            "U B U B' U' B' R B2 U' B' U' B U B' R'", '', '', '', '', '', '', '', '', '', '', '', '', "B2 R2 B' L' B R2 B' L B'", "U' L' U' L F2 D R' U R U' R D' F2", '', "U' L2 D' B U' B U B' D L2 F U' F'", '', "U2 L U2 L' U2 L F' L' U' L U L F L2", '', "U2 B' U' B L2 D F' U F U' F D' L2", "U2 F' U' L' F U F' U' F' L F2 U' F' U' F U F' U F", '', '', '', "U R2 D B' U B' U' B D' R2 F' U F", 
            "U L U L' U' L' B L2 U' L' U' L U L' B'", '', '', '', '', "U2 B2 R B U B U' B' R' B U2 B' U2 B", '', '', "U L2 B2 L' F' L B2 L' F L'", "U R U R' F' R U R' U' R' F R2 U' R'", '', "F U F' L' F U F' U' F' L F2 U' F'", '', '', "U2 B2 D' R U' R U R' D B2 L U' L'", '', "U' F U2 F' U2 F R' F' U' F U F R F2", "U2 F U F' U' F' L F2 U' F' U' F U F' L'", '', '', "U L2 D L D' L B2 R' U R B2", 
            '', '', '', '', "U2 R B' R F2 R' B R F2 R2", "U2 F2 D F D' F L2 B' U B L2", '', '', '', "U' B L' B R2 B' L B R2 B2", "F2 D R' U R' U' R D' F2 L' U L", '', "F U F' L2 D' B U' B' U B' D L2", '', "U R U R' F2 D' L U' L' U L' D F2", '', "U' L2 B L U L U' L' B' L U2 L' U2 L", "U L' U' B' L U L' U' L' B L2 U' L' U' L U L' U L", '', '', '', '', '', '', '', '', '', '', '', '', 
            "R B' R F2 R' B R F2 R2", "U L2 D F' U F' U' F D' L2 B' U B", '', "U L U L' B2 D' R U' R' U R' D B2", '', "U2 F U F' L2 D' B U' B' U B' D L2", '', "B2 R B U B U' B' R' B U2 B' U2 B", "U2 B' U' R' B U B' U' B' R B2 U' B' U' B U B' U B", '', '', '', "U' R' U' R B2 D L' U L U' L D' B2", "U' L' U' B' L U L' U' L' B L2 U' L' U' L U L' U L", '', '', '', '', "B' U' B L2 D F' U F U' F D' L2", 
            '', '', "U R2 F2 R' B' R F2 R' B R'", "U' B U2 B' U2 B L' B' U' B U B L B2", '', "B2 D' R U' R U R' D B2 L U' L'", '', '', "U2 F U F' L' F U F' U' F' L F2 U' F'", '', "U L U L' B' L U L' U' L' B L2 U' L'", "U2 B U B' U' B' R B2 U' B' U' B U B' R'", '', '', "U2 F2 D R' U R' U' R D' F2 L' U L", '', '', '', '', "U2 B2 R2 B' L' B R2 B' L B'", "U' R2 F R U R U' R' F' R U2 R' U2 R", 
            '', '', '', "U' F R' F L2 F' R F L2 F2", "U' L2 D L D' L B2 R' U R B2", '', "L U2 L' U2 L F' L' U' L U L F L2", '', "U' R2 D' F U' F U F' D R2 B U' B'", '', "U2 B2 D B D' B R2 F' U F R2", "U' L U L' U' L' B L2 U' L' U' L U L' B'", '', '', '', '', '', '', '', '', '', '', '', '', "L2 B2 L' F' L B2 L' F L'", "B' U B' U' L' B' L2 U' L' U L' B L B", '', "R B' R B2 U' B' U' B U B' R' B U B' U' R'", 
            '', "U F2 D F D' F L2 B' U B L2", '', "R U R' F2 D' L U' L' U L' D F2", "U' F' U B' U2 F U' B F' U B' U2 F U' B", '', '', '', "F' U F' U' R' F' R2 U' R' U R' F R F", "U F' U' L' F U F' U' F' L F2 U' F' U' F U F' U F", '', '', '', '', "U2 F U2 F' U2 F R' F' U' F U F R F2", '', '', "U B L' B' R B L B' R' B L B' R B L' B' R'", "U2 L' U L' U' F' L' F2 U' F' U F' L F L", '', 
            "U2 L2 B L U L U' L' B' L U2 L' U2 L", '', '', "U2 L' U' L F2 D R' U R U' R D' F2", '', "U2 R' U R' U' B' R' B2 U' B' U B' R B R", "U' F U' B U2 F' U B' F U' B U2 F' U B'", '', '', "R U R' F' R U R' U' R' F R2 U' R'", '', '', '', '', "U2 B L' B R2 B' L B R2 B2", "U2 B L' B L2 U' L' U' L U L' B' L U L' U' B'", '', '', '', "U' R B' R' F R B R' F' R B R' F R B' R' F'", 
            "U2 L2 D' B U' B U B' D L2 F U' F'", '', "R2 D B' U B' U' B D' R2 F' U F", '', "L F' L F2 U' F' U' F U F' L' F U F' U' L'", '', "U2 F R' F R2 U' R' U' R U R' F' R U R' U' F'", "U F U F' U' F' L F2 U' F' U' F U F' L'", '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', 
            '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', 'U2 L2 R2 D L2 R2 U2 L2 R2 D L2 R2', "U B U B' R2 D' F U' F' U F' D R2", '', "U' F' U' F R2 D B' U B U' B D' R2", '', "U2 L U' L U L U L U' L' U' L2", '', "U2 R2 U R U R' U' R' U' R' U R'", "U2 R U R' U' R' F R2 U' R' U' R U R' F'", 
            '', '', '', "U' F2 D' L U' L U L' D F2 R U' R'", "U F B' R F2 B2 L F2 B2 R F B' D2 F2 B2", '', '', '', '', "U2 F U' F U F U F U' F' U' F2", '', '', "U F2 L2 F' R' F L2 F' R F'", "U' F2 L F U F U' F' L' F U2 F' U2 F", '', "U2 R U' R U R U R U' R' U' R2", '', '', "U2 B2 U B U B' U' B' U' B' U B'", '', "U2 R2 D R D' R F2 L' U L F2", "U2 R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R", '', '', 
            "U2 B U' B U B U B U' B' U' B2", '', '', '', '', 'U2', "U B U B' R' B U B' U' B' R B2 U' B'", '', '', '', "U' L F' L B2 L' F L B2 L2", "U2 L2 U L U L' U' L' U' L' U L'", '', "U2 F2 U F U F' U' F' U' F' U F'", '', "U B2 D L' U L' U' L D' B2 R' U R", '', "U' R U2 R' U2 R B' R' U' R U R B R2", "U L R' F L2 R2 B L2 R2 F L R' D2 L2 R2", '', '', '', '', '', '', '', '', '', '', '', '', 
            "B L' B R2 B' L B R2 B2", "U' B2 U B U B' U' B' U' B' U B'", '', "U' L2 U L U L' U' L' U' L' U L'", '', "U2 R2 D B' U B' U' B D' R2 F' U F", '', "F U2 F' U2 F R' F' U' F U F R F2", "U2 F B' R F2 B2 L F2 B2 R F B' D2 F2 B2", '', '', '', "U' F2 U F U F' U' F' U' F' U F'", "U' F U F' U' F' L F2 U' F' U' F U F' L'", '', '', '', '', "U2 R U R' F2 D' L U' L' U L' D F2", '', '', 
            "U' L2 R2 D L2 R2 U2 L2 R2 D L2 R2", "U' B U' B U B U B U' B' U' B2", '', "L' U' L F2 D R' U R U' R D' F2", '', '', "L2 B L U L U' L' B' L U2 L' U2 L", '', "U' F U' F U F U F U' F' U' F2", "U2 L R' F L2 R2 B L2 R2 F L R' D2 L2 R2", '', '', "L2 D' B U' B U B' D L2 F U' F'", '', '', '', '', "U2 L2 B2 L' F' L B2 L' F L'", "U' L U' L U L U L U' L' U' L2", '', '', '', "U'", 
            "U2 R U R' F' R U R' U' R' F R2 U' R'", '', "U' F2 D F D' F L2 B' U B L2", '', "U' R2 U R U R' U' R' U' R' U R'", '', "U' R U' R U R U R U' R' U' R2", "U' F' U' L' F U F' U' F' L F2 U' F' U' F U F' U F", '', '', '', '', '', '', '', '', '', '', '', '', "L F' L B2 L' F L B2 L2", "B2 D B D' B R2 F' U F R2", '', "U B U2 B' U2 B L' B' U' B U B L B2", '', "F2 D' L U' L U L' D F2 R U' R'", '', 
            "U' R2 D R D' R F2 L' U L F2", "B U B' U' B' R B2 U' B' U' B U B' R'", '', '', '', "U R2 F R U R U' R' F' R U2 R' U2 R", "U' R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R", '', '', '', '', "U2 B2 D L' U L' U' L D' B2 R' U R", '', '', "U F R' F L2 F' R F L2 F2", "U' L U L' B2 D' R U' R' U R' D B2", '', "U2 B U B' R2 D' F U' F' U F' D R2", '', '', "R U2 R' U2 R B' R' U' R U R B R2", '', 
            "U R2 D' F U' F U F' D R2 B U' B'", "B' U' R' B U B' U' B' R B2 U' B' U' B U B' U B", '', '', "F' U' F R2 D B' U B U' B D' R2", '', '', '', '', "U2 F2 L2 F' R' F L2 F' R F'", "U R' U' R B2 D L' U L U' L D' B2", '', '', '', "U' R2 F2 R' B' R F2 R' B R'", "F2 L F U F U' F' L' F U2 F' U2 F", '', "U2 B U B' R' B U B' U' B' R B2 U' B'", '', "U' L U L' B' L U L' U' L' B L2 U' L'", '', 
            "U' L2 D F' U F' U' F D' L2 B' U B", "U' R U R' U' R' F R2 U' R' U' R U R' F'", '', '', '', '', '', '', '', '', '', '', '', '', "B L' B' R B L B' R' B L B' R B L' B' R'", "U' B2 D' R U' R U R' D B2 L U' L'", '', "U F2 D R' U R' U' R D' F2 L' U L", '', "U B L' B L2 U' L' U' L U L' B' L U L' U' B'", '', "U' L F' L F2 U' F' U' F U F' L' F U F' U' L'", "U2 L U L' U' L' B L2 U' L' U' L U L' B'", 
            '', '', '', "U F U F' L2 D' B U' B' U B' D L2", "L' U R' U2 L U' R L' U R' U2 L U' R", '', '', '', '', "U R' U R' U' B' R' B2 U' B' U B' R B R", '', '', "U B2 R2 B' L' B R2 B' L B'", "U2 L2 D L D' L B2 R' U R B2", '', "U F R' F R2 U' R' U' R U R' F' R U R' U' F'", '', '', "U' B' U B' U' L' B' L2 U' L' U L' B L B", '', "U' B2 R B U B U' B' R' B U2 B' U2 B", "U2 L' U' B' L U L' U' L' B L2 U' L' U' L U L' U L", 
            '', '', "U L' U L' U' F' L' F2 U' F' U F' L F L", '', '', '', '', "U2 R B' R' F R B R' F' R B R' F R B' R' F'", "U' L U2 L' U2 L F' L' U' L U L F L2", '', '', '', "U' R B' R F2 R' B R F2 R2", "U' R B' R B2 U' B' U' B U B' R' B U B' U' R'", '', "U' F' U F' U' R' F' R2 U' R' U R' F R F", '', "U' B' U' B L2 D F' U F U' F D' L2", '', "U F U F' L' F U F' U' F' L F2 U' F'", "R U' L U2 R' U L' R U' L U2 R' U L'")

########################################  FUNCTIONS  ###########################################################
########################################  CUBE DATA INTERFACE
def CI_to_int(CI: tuple[int, int, int]) -> np.uint8:
    # number is uint8 11ZZYYXX, where XX, YY, and ZZ are x, y, and z each + 1
    return np.uint8(0b11000000 + ((CI[2]+1)<<4) + ((CI[1]+1)<<2) + (CI[0]+1))
def int_to_CI(INT: np.uint8) -> tuple[int, int, int]:
    return (int((INT&0b11))-1, int((INT&0b1111)>>2)-1, int((INT&0b111111)>>4)-1)
def cube3d_from_cube2ds(cube2ds):
    # x, y, z point right, up, and back, respectively.
    cube3d = np.zeros(shape=(5, 5, 5), dtype=np.uint8)
    for i in (-1, 0, 1):
        for j in (-1, 0, 1):
            cube3d[i+2, 4, j+2] = faces_to_nums[cube2ds['U'][(1-j, i+1)]]
            cube3d[4, j+2, i+2] = faces_to_nums[cube2ds['R'][(1-j, i+1)]]
            cube3d[i+2, j+2, 0] = faces_to_nums[cube2ds['F'][(1-j, i+1)]]
            cube3d[0, i+2, j+2] = faces_to_nums[cube2ds['L'][(1-i, 1-j)]]
            cube3d[i+2, j+2, 4] = faces_to_nums[cube2ds['B'][(1-j, 1-i)]]
            cube3d[i+2, 0, j+2] = faces_to_nums[cube2ds['D'][(1-j, 1-i)]]
    # corners
    for x, y, z in ((-1, -1, -1), (-1, -1, 1), (-1, 1, -1), (-1, 1, 1), (1, -1, -1), (1, -1, 1), (1, 1, -1), (1, 1, 1)):
        corner = {cube3d[2*x+2, y+2, z+2],
                  cube3d[x+2, 2*y+2, z+2],
                  cube3d[x+2, y+2, 2*z+2]}
        
        if corner == {1, 2, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((1, 1, -1))
        if corner == {2, 3, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, 1, -1))
        if corner == {3, 4, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, 1, 1))
        if corner == {4, 1, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((1, 1, 1))

        if corner == {1, 4, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((1, -1, 1))
        if corner == {2, 1, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((1, -1, -1))
        if corner == {3, 2, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, -1, -1))
        if corner == {4, 3, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, -1, 1))
    # edges
    for x, y, z in ((-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0),
               (-1, 0, -1), (-1, 0, 1), (1, 0, -1), (1, 0, 1),
               (0, -1, -1), (0, -1, 1), (0, 1, -1), (0, 1, 1)):
        if x==0: edge = {cube3d[2, 2*y+2, z+2], cube3d[2, y+2, 2*z+2]}
        if y==0: edge = {cube3d[2*x+2, 2, z+2], cube3d[x+2, 2, 2*z+2]}
        if z==0: edge = {cube3d[2*x+2, y+2, 2], cube3d[x+2, 2*y+2, 2]}

        if edge == {1, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((1, 1, 0))
        if edge == {2, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((0, 1, -1))
        if edge == {3, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, 1, 0))
        if edge == {4, 0}: cube3d[x+2, y+2, z+2] = CI_to_int((0, 1, 1))

        if edge == {1, 2}: cube3d[x+2, y+2, z+2] = CI_to_int((1, 0, -1))
        if edge == {2, 3}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, 0, -1))
        if edge == {3, 4}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, 0, 1))
        if edge == {4, 1}: cube3d[x+2, y+2, z+2] = CI_to_int((1, 0, 1))

        if edge == {1, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((1, -1, 0))
        if edge == {2, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((0, -1, -1))
        if edge == {3, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((-1, -1, 0))
        if edge == {4, 5}: cube3d[x+2, y+2, z+2] = CI_to_int((0, -1, 1))
    # centers
    for x, y, z in ((1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1)):
        cube3d[x+2, y+2, z+2] = CI_to_int((x, y, z))
    return cube3d
def cube2ds_from_cube3d(cube3d):
    cube2ds = {'U': np.array([['N', 'N', 'N'], 
                        ['N', 'N', 'N'], 
                        ['N', 'N', 'N']]), 
            'R': np.array([['N', 'N', 'N'], 
                        ['N', 'N', 'N'], 
                        ['N', 'N', 'N']]),
            'F': np.array([['N', 'N', 'N'], 
                        ['N', 'N', 'N'], 
                        ['N', 'N', 'N']]),
            'L': np.array([['N', 'N', 'N'], 
                        ['N', 'N', 'N'], 
                        ['N', 'N', 'N']]),  
            'B': np.array([['N', 'N', 'N'], 
                        ['N', 'N', 'N'], 
                        ['N', 'N', 'N']]),
            'D': np.array([['N', 'N', 'N'], 
                        ['N', 'N', 'N'], 
                        ['N', 'N', 'N']])}
    for i in (-1, 0, 1):
        for j in (-1, 0, 1):
            cube2ds['U'][(1-j, i+1)] = faces[cube3d[i+2, 4, j+2]]
            cube2ds['R'][(1-j, i+1)] = faces[cube3d[4, j+2, i+2]]
            cube2ds['F'][(1-j, i+1)] = faces[cube3d[i+2, j+2, 0]]
            cube2ds['L'][(1-i, 1-j)] = faces[cube3d[0, i+2, j+2]]
            cube2ds['B'][(1-j, 1-i)] = faces[cube3d[i+2, j+2, 4]]
            cube2ds['D'][(1-j, 1-i)] = faces[cube3d[i+2, 0, j+2]]
    return cube2ds
def printcube(cube3d):
    cube = cube2ds_from_cube3d(cube3d)
    for key, value in cube.items():
        print(f'{key}: ')
        print(value)
    print('-------------------------------------')
######################################### CUBE MANIPULATION
def update_cube_with_F_move(cube3d, F_move: str) -> None:
    Fmove = F_move[0]
    c = {"'": -1, "2": 2}.get(F_move[-1], 1)
    if Fmove == 'U': cube3d[:,3:,:] = np.rot90(cube3d, k=c, axes=(2, 0))[:,3:,:]
    if Fmove == 'R': cube3d[3:,:,:] = np.rot90(cube3d, k=c, axes=(1, 2))[3:,:,:]
    if Fmove == 'F': cube3d[:,:,:2] = np.rot90(cube3d, k=c, axes=(1, 0))[:,:,:2]
    if Fmove == 'L': cube3d[:2,:,:] = np.rot90(cube3d, k=c, axes=(2, 1))[:2,:,:]
    if Fmove == 'B': cube3d[:,:,3:] = np.rot90(cube3d, k=c, axes=(0, 1))[:,:,3:]
    if Fmove == 'D': cube3d[:,:2,:] = np.rot90(cube3d, k=c, axes=(0, 2))[:,:2,:]
def algorithm_from_Fs_perspective(face, algorithm):
    return [f'{faces[move_from_Fs_perspective[(faces_to_nums[face], faces_to_nums[move[0]])]]}{(move[-1]*(len(move)==2))}'
            for move in algorithm.split()]
######################################### CUBE SOLVING
################# C
def get_algs_for_bottom_edges(cube3d) -> tuple[str, int, int]:
    algorithms = []
    for x0, y0, z0 in ((-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0),
               (-1, 0, -1), (-1, 0, 1), (1, 0, -1), (1, 0, 1),
               (0, -1, -1), (0, -1, 1), (0, 1, -1), (0, 1, 1)):
        if x0==0: xyzs = ((0, 2*y0, z0), (0, y0, 2*z0))
        if y0==0: xyzs = ((2*x0, 0, z0), (x0, 0, 2*z0))
        if z0==0: xyzs = ((2*x0, y0, 0), (x0, 2*y0, 0))
        for x, y, z in xyzs:
            if cube3d[x+2, y+2, z+2] != faces_to_nums['D']: continue # make sure it is a bottom edge
            if y == -2:
                destination = int_to_CI(cube3d[x+2, 1, z+2])
                if destination == (x, -1, z): continue               # make sure it is wrong
            if y == 2: destination = int_to_CI(cube3d[x+2, 3, z+2])
            if abs(x) == 2: destination = int_to_CI(cube3d[x//2 + 2, y+2, z+2])
            if abs(z) == 2: destination = int_to_CI(cube3d[x+2, y+2, z//2 + 2])
            if destination == (1, -1, 0): algorithms.append(algorithm_from_Fs_perspective('R', FULL_CROSS[piece_indexes[(z+2, y+2, -x+2)]]))
            if destination == (0, -1, -1): algorithms.append(algorithm_from_Fs_perspective('F', FULL_CROSS[piece_indexes[(x+2, y+2, z+2)]]))
            if destination == (-1, -1, 0): algorithms.append(algorithm_from_Fs_perspective('L', FULL_CROSS[piece_indexes[(-z+2, y+2, x+2)]]))
            if destination == (0, -1, 1): algorithms.append(algorithm_from_Fs_perspective('B', FULL_CROSS[piece_indexes[(-x+2, y+2, -z+2)]]))
    return algorithms
################# F
def get_algs_for_f2l_pairs(cube3d):
    cornerFR = None
    cornerLF = None
    cornerBL = None
    cornerRB = None
    edgeFR = None
    edgeLF = None
    edgeBL = None
    edgeRB = None
    # corners
    for x, y, z in ((-1, -1, -1), (-1, -1, 1), (-1, 1, -1), (-1, 1, 1), 
                    (1, -1, -1), (1, -1, 1), (1, 1, -1), (1, 1, 1)):
        corner_coords = ((2*x, y, z), (x, 2*y, z), (x, y, 2*z))
        corner_colors = tuple(map(lambda x: cube3d[x[0]+2,x[1]+2,x[2]+2], corner_coords))
        if set(corner_colors) == {1, 4, 5}: 
            for i in range(3):
                if corner_colors[i] == 1: cornerRB = corner_coords[i]
        if set(corner_colors) == {2, 1, 5}: 
            for i in range(3):
                if corner_colors[i] == 2: cornerFR = corner_coords[i]
        if set(corner_colors) == {3, 2, 5}: 
            for i in range(3):
                if corner_colors[i] == 3: cornerLF = corner_coords[i]
        if set(corner_colors) == {4, 3, 5}: 
            for i in range(3):
                if corner_colors[i] == 4: cornerBL = corner_coords[i]
    # edges
    for x, y, z in ((-1, 1, 0), (1, 1, 0), (-1, 0, -1), (-1, 0, 1), 
                    (1, 0, -1), (1, 0, 1), (0, 1, -1), (0, 1, 1)):
        if x==0: edge_coords = ((0, 2*y, z), (0, y, 2*z))
        if y==0: edge_coords = ((2*x, 0, z), (x, 0, 2*z))
        if z==0: edge_coords = ((2*x, y, 0), (x, 2*y, 0))
        edge_colors = tuple(map(lambda x: cube3d[x[0]+2,x[1]+2,x[2]+2], edge_coords))
        if set(edge_colors) == {1, 2}: 
            for i in range(2):
                if edge_colors[i] == 2: edgeFR = edge_coords[i]
        if set(edge_colors) == {2, 3}: 
            for i in range(2):
                if edge_colors[i] == 3: edgeLF = edge_coords[i]
        if set(edge_colors) == {3, 4}: 
            for i in range(2):
                if edge_colors[i] == 4: edgeBL = edge_coords[i]
        if set(edge_colors) == {4, 1}: 
            for i in range(2):
                if edge_colors[i] == 1: edgeRB = edge_coords[i]

    f2l_pairs = []
    if cornerFR != (1, -1, -2) or edgeFR != (1, 0, -2): 
        f2l_pairs.append(((lambda x: (x[0], x[1], x[2]))(cornerFR), 
                          (lambda x: (x[0], x[1], x[2]))(edgeFR), 'F'))
    if cornerLF != (-2, -1, -1) or edgeLF != (-2, 0, -1): 
        f2l_pairs.append(((lambda x: (-x[2], x[1], x[0]))(cornerLF), 
                          (lambda x: (-x[2], x[1], x[0]))(edgeLF), 'L'))
    if cornerBL != (-1, -1, 2) or edgeBL != (-1, 0, 2): 
        f2l_pairs.append(((lambda x: (-x[0], x[1], -x[2]))(cornerBL), 
                          (lambda x: (-x[0], x[1], -x[2]))(edgeBL), 'B'))
    if cornerRB != (2, -1, 1) or edgeRB != (2, 0, 1): 
        f2l_pairs.append(((lambda x: (x[2], x[1], -x[0]))(cornerRB), 
                          (lambda x: (x[2], x[1], -x[0]))(edgeRB), 'R'))
    algorithms = []
    for x in f2l_pairs:
        algs = FULL_F2L[piece_indexes[(x[0][0]+2, x[0][1]+2, x[0][2]+2)]][piece_indexes[(x[1][0]+2, x[1][1]+2, x[1][2]+2)]]
        #print(type(algs[0]))
        for alg in algs: 
            algorithms.append(algorithm_from_Fs_perspective(x[2], alg))
    return algorithms
    #'''
################# O
def get_OLL_algorithm(cube3d):
    top_layer_num = 0
    for x, y, z in ((2, 1, 0), (0, 1, -2), (-2, 1, 0), (2, 1, 1),(2, 1, -1),
                    (1, 1, -2),(-1, 1, -2), (-2, 1, -1),(1, 1, 2)):
        top_layer_num = (top_layer_num*2)|(cube3d[x+2, y+2, z+2]==0)
    return FULL_OLL[top_layer_num].split()
################# P
def get_PLL_algorithm(cube3d):
    return FULL_PLL[(int(cube3d[4, 3, 3])-1)*256
                  + (int(cube3d[3, 3, 0])-1)*64
                  + (int(cube3d[4, 3, 2])-1)*16
                  + (int(cube3d[2, 3, 0])-1)*4
                  + (int(cube3d[0, 3, 2])-1)].split()
########################### Recursive Search for optimal CFOP
def OP(F_moves, cube3d, shortest_length): # At the end of Recursive_F, OP is called, thereby completing the recursive CFOP
    cube3dnew = np.copy(cube3d)
    algorithmOLL = get_OLL_algorithm(cube3dnew)
    if len(simplify_FURDLB_MOVES(F_moves + algorithmOLL)) - 1 >= shortest_length[0]: return False
    for move in algorithmOLL: update_cube_with_F_move(cube3dnew, move)
    SOLVE_SEQUENCE = simplify_FURDLB_MOVES(F_moves + algorithmOLL + get_PLL_algorithm(cube3dnew))
    if len(SOLVE_SEQUENCE) < shortest_length[0]: shortest_length[0] = len(SOLVE_SEQUENCE)
    return SOLVE_SEQUENCE
def Recursive_F(F_moves, cube3d, shortest_length): # At the end of Recursive_C, Recursive_F is called
    algorithms = get_algs_for_f2l_pairs(cube3d)
    best = None
    if not algorithms: 
        #print(simplify_FURDLB_MOVES(F_moves))
        return OP(F_moves, cube3d, shortest_length)
    for alg in algorithms:
        if len(simplify_FURDLB_MOVES(F_moves + alg)) - 1 >= shortest_length[0]: continue
        cube3dnew = np.copy(cube3d)
        for move in alg: update_cube_with_F_move(cube3dnew, move)
        candidate = Recursive_F(F_moves + alg, cube3dnew, shortest_length)
        if candidate is not None and candidate != False:
            if best is None or len(candidate) < len(best):
                best = candidate
    return best
def Recursive_C(F_moves, cube3d, shortest_length):
    algorithms = get_algs_for_bottom_edges(cube3d)
    best = None
    if not algorithms: 
        #print(simplify_FURDLB_MOVES(F_moves))
        return Recursive_F(F_moves, cube3d, shortest_length)
    for alg in algorithms:
        if len(simplify_FURDLB_MOVES(F_moves + alg)) - 1 >= shortest_length[0]: continue
        cube3dnew = np.copy(cube3d)
        for move in alg: update_cube_with_F_move(cube3dnew, move)
        candidate = Recursive_C(F_moves + alg, cube3dnew, shortest_length)
        if best is None or (candidate is not None and len(candidate) < len(best)):
            best = candidate
    return best
#######################################################################################################################
def simplify_FURDLB_MOVES(MOVES) -> list[str]:
    mods_to_nums = {"'": -1,
                    "2": 2}
    nums_to_mods = ["0", "", "2", "'"]
    while True:
        new = []
        modified = False
        i = 0
        while i < len(MOVES):
            if i >= len(MOVES)-1: 
                new.append(MOVES[i])
                i += 1
                continue
            v1, v2 = MOVES[i:i+2]
            if v1[0] == v2[0]:
                modified = True
                sum_of_modifiers = nums_to_mods[(mods_to_nums.get(v1[-1], 1) + mods_to_nums.get(v2[-1], 1))%4]
                if sum_of_modifiers != "0": new.append(f"{v1[0]}{sum_of_modifiers}")
                i += 2
            else: 
                new.append(v1)
                i += 1
        MOVES = new[:]
        new = []
        i = 0
        while i < len(MOVES):
            if i >= len(MOVES)-2: 
                new.append(MOVES[i])
                i += 1
                continue
            v1, v2, v3 = MOVES[i:i+3]
            if v1[0] == v3[0] and (v1[0], v2[0]) in {('U', 'D'), ('D', 'U'), ('F', 'B'), ('B', 'F'), ('R', 'L'), ('L', 'R')}:
                modified = True
                sum_of_modifiers = nums_to_mods[(mods_to_nums.get(v1[-1], 1) + mods_to_nums.get(v3[-1], 1))%4]
                if sum_of_modifiers != "0": new.append(f"{v1[0]}{sum_of_modifiers}")
                new.append(v2)
                i += 3
            else: 
                new.append(v1)
                i += 1
        MOVES = new[:]
        if not modified: return MOVES
######################################### RUN #########################################################################
def CUBE3D_from_scramble(scramble):
    cube3d = cube3d_from_cube2ds({'U': np.array([['U', 'U', 'U'], 
                                                ['U', 'U', 'U'], 
                                                ['U', 'U', 'U']]),
                                'R': np.array([['R', 'R', 'R'], 
                                                ['R', 'R', 'R'], 
                                                ['R', 'R', 'R']]),
                                'F': np.array([['F', 'F', 'F'], 
                                                ['F', 'F', 'F'], 
                                                ['F', 'F', 'F']]),
                                'L': np.array([['L', 'L', 'L'], 
                                                ['L', 'L', 'L'], 
                                                ['L', 'L', 'L']]),  
                                'B': np.array([['B', 'B', 'B'], 
                                                ['B', 'B', 'B'], 
                                                ['B', 'B', 'B']]),
                                'D': np.array([['D', 'D', 'D'], 
                                                ['D', 'D', 'D'], 
                                                ['D', 'D', 'D']])})
    for move in algorithm_from_Fs_perspective('F', scramble):
        update_cube_with_F_move(cube3d, move)
    return cube3d
def algorithm_inverter(algorithm: str):
    alg = algorithm.split()
    inverse = []
    for move in alg[::-1]:
        mv = move[0]
        if move[-1] == "2":inverse.append(f'{mv}2')
        elif move[-1] == "'":inverse.append(f'{mv}')
        else: inverse.append(f"{mv}'")
    return ' '.join(inverse)
scrambles = ["F D' R2 D' L' F L B' U R D' R F' U2 F D R U' F' D2 L U' R2 B' U2",
             "L' B R2 F2 L' B L' D' F' L' D2 R' B' R F R' F R F U L B L U' R'",
             "D F L U B' U' L2 B' L' B' U' R' D F' D' L2 D F L U L' D2 L U L'",
             "B2 D R' F' R2 B' D2 L2 D B2 D L' F D2 L2 D L' F' R2 U L' D' F U B'",
             "R' D F L' D' R' D F2 R' F' R' B' R F2 R B' U F' L' D B2 L' D L' F",
             "L' B D F' L' B D2 B L' B' D L' U B L D R' B2 R D2 R U L D' B",
             "D B' L' D F' R' D L F2 U F D' L F' L' F' D' L U' B D R B' U2 F",
             "L2 D R2 F D R2 U2 R' F' R' F' L F D R B' U R' U F' D B' R' B R'",
             "F2 U L' U R' U L U B' L F D' F' U' R' D F2 R B' L D2 B' L' F' L'"]

move_counts = []
for scramble in scrambles:
    start = time.perf_counter()
    CUBE3D = CUBE3D_from_scramble(scramble)
    #printcube(CUBE3D)
    F_MOVES = Recursive_C([], CUBE3D, [np.inf])
    for move in F_MOVES: update_cube_with_F_move(CUBE3D, move)
    if not np.array_equal(CUBE3D, cube3d_from_cube2ds({'U': np.array([['U', 'U', 'U'], 
                                                ['U', 'U', 'U'], 
                                                ['U', 'U', 'U']]),
                                'R': np.array([['R', 'R', 'R'], 
                                                ['R', 'R', 'R'], 
                                                ['R', 'R', 'R']]),
                                'F': np.array([['F', 'F', 'F'], 
                                                ['F', 'F', 'F'], 
                                                ['F', 'F', 'F']]),
                                'L': np.array([['L', 'L', 'L'], 
                                                ['L', 'L', 'L'], 
                                                ['L', 'L', 'L']]),  
                                'B': np.array([['B', 'B', 'B'], 
                                                ['B', 'B', 'B'], 
                                                ['B', 'B', 'B']]),
                                'D': np.array([['D', 'D', 'D'], 
                                                ['D', 'D', 'D'], 
                                                ['D', 'D', 'D']])})):
        print("AAAAAAAAAAAAAAAAAAAAAAAAAA")
        printcube(CUBE3D)
    print(scramble)
    print(' '.join(F_MOVES))
    print(f'move count: {len(F_MOVES)}')
    move_counts.append(len(F_MOVES))
    t = time.perf_counter() - start
    print(f"{t} s")

print(f'average move count: {sum(move_counts)/len(move_counts)}')
