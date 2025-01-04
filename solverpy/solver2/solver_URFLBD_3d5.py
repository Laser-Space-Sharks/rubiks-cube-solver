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

F_MOVES = []
    
#################################  GENERIC OPERATION CORRESPONDANCES  #######################################
                                    # U  R  F  L  B  D
move_from_Fs_perspective = np.array([[4, 1, 0, 3, 5, 2],  # U
                                     [0, 4, 1, 2, 3, 5],  # R
                                     [0, 1, 2, 3, 4, 5],  # F
                                     [0, 2, 3, 4, 1, 5],  # L
                                     [0, 3, 4, 1, 2, 5],  # B
                                     [4, 3, 5, 1, 0, 2]]) # D



######################################  ALGORITHMS  ###############################################
#                                                          'D'  'F'
bottom_cross_algorithms = {(1, 2, 0): "U, F2",          # ('U', 'R')
                           (0, 2, -1): "F2",            # ('U', 'F')
                           (-1, 2, 0): "U', F2",        # ('U', 'L')
                           (0, 2, 1): "U2, F2",         # ('U', 'B')

                           (2, 1, 0): "R', F, R",       # ('R', 'U')
                           (2, 0, -1): "F",             # ('R', 'F')
                           (2, 0, 1): "D2, B', D2",     # ('R', 'B')
                           (2, -1, 0): "R, F",          # ('R', 'D')

                           (0, 1, -2): "F, D, R', D'",  # ('F', 'U')
                           (1, 0, -2): "D, R', D'",     # ('F', 'R')
                           (-1, 0, -2): "D', L, D",     # ('F', 'L')
                           (0, -1, -2): "F, D', L, D",  # ('F', 'D')

                           (-2, 1, 0): "L, F', L'",     # ('L', 'U')
                           (-2, 0, -1): "F'",           # ('L', 'F')
                           (-2, 0, 1): "D2, B, D2",     # ('L', 'B')
                           (-2, -1, 0): "L', F'",       # ('L', 'D')

                           (0, 1, 2): "U, R', F, R",    # ('B', 'U')
                           (1, 0, 2): "D, R, D'",       # ('B', 'R')
                           (-1, 0, 2): "D', L', D",     # ('B', 'L')
                           (0, -1, 2): "B, D, R, D'",   # ('B', 'D')

                           (1, -2, 0): "R2, U, F2",     # ('D', 'R')
                           (0, -2, -1): "AAAAAAAA",     # ('D', 'F')
                           (-1, -2, 0): "L2, U', F2",   # ('D', 'L')
                           (0, -2, 1): "B2, U2, F2"}    # ('D', 'B')


corners_to_indices = {(-2, 1, 1): 0,     # ('L', 'B', 'U')
                      (-1, 1, 2): 1,     # ('B', 'U', 'L')
                      (-1, 2, 1): 2,     # ('U', 'L', 'B')
                      (1, 1, 2): 3,     # ('B', 'R', 'U')
                      (2, 1, 1): 4,     # ('R', 'U', 'B')
                      (1, 2, 1): 5,     # ('U', 'B', 'R')
                      (2, 1, -1): 6,     # ('R', 'F', 'U')
                      (1, 1, -2): 7,     # ('F', 'U', 'R')
                      (1, 2, -1): 8,     # ('U', 'R', 'F')
                      (-1, 1, -2): 9,     # ('F', 'L', 'U')
                      (-2, 1, -1): 10,    # ('L', 'U', 'F')
                      (-1, 2, -1): 11,    # ('U', 'F', 'L')
                      (-1, -1, 2): 12,    # ('B', 'L', 'D')
                      (-2, -1, 1): 13,    # ('L', 'D', 'B')
                      (-1, -2, 1): 14,    # ('D', 'B', 'L')
                      (2, -1, 1): 15,    # ('R', 'B', 'D')
                      (1, -1, 2): 16,    # ('B', 'D', 'R')
                      (1, -2, 1): 17,    # ('D', 'R', 'B')
                      (1, -1, -2): 18,    # ('F', 'R', 'D')
                      (2, -1, -1): 19,    # ('R', 'D', 'F')
                      (1, -2, -1): 20,    # ('D', 'F', 'R')
                      (-2, -1, -1): 21,    # ('L', 'F', 'D')
                      (-1, -1, -2): 22,    # ('F', 'D', 'L')
                      (-1, -2, -1): 23}    # ('D', 'L', 'F')
edges_to_indices = {(0, 2, 1): 0,
                   (0, 1, 2): 1,
                   (1, 2, 0): 2,
                   (2, 1, 0): 3,
                   (0, 2, -1): 4,
                   (0, 1, -2): 5,
                   (-1, 2, 0): 6,
                   (-2, 1, 0): 7,
                   (-1, 0, 2): 8,
                   (-2, 0, 1): 9,
                   (1, 0, 2): 10,
                   (2, 0, 1): 11,
                   (1, 0, -2): 12,
                   (2, 0, -1): 13,
                   (-1, 0, -2): 14,
                   (-2, 0, -1): 15}
F2L_ALGORITHMS = [["U' F R' F' R U R U R'", "U2 F' U2 F U F' U' F", "U2 R B U2 B' R'", "U F' U2 F U' F' U F", "U' R U2 R' U R U' R'", "U2 F' L' U2 L F", "U2 R U2 R' U' R U R'", "U R' F R F' U' F' U' F", "U2 R2 D B2 D' R2", "R B' U B R'", "U R' U R2 U' R'", "U2 B' R B2 U' B' U2 R'", "U' R U' R' U R U' R' U R U' R'", "U2 R U' R' F' U2 F", "U' F U' F2 U F", "U2 L F' L2 U L U2 F"],
["U2 R' U2 R2 U R2 U R", "U F' U F", "U R U R' U R U R'", "U' F' U' F U2 F' U F", "U2 R U R'", "U2 L' B2 R2 B R2 B L", "U R U' R' U R U R'", "U2 R U' R' U2 F' U' F", "U L U L' R U R'", "U2 B' R U R' B", "U2 R' U' R2 U R'", "U2 F D R D' F'", "U' R U R' U2 R U R'", "U' F' U' F U' R U R'", "U L' U' L R U' R'", "U2 F U2 F' R U R'"],
["U2 F' U F U2 R U R'", "U' F' U F U' F' U' F", "U2 B L2 F2 L' F2 L' B'", "U2 F' U' F", "U R U R' U2 R U' R'", "U' F' U' F U' F' U' F", "U' R U' R'", "U2 F U2 F2 U' F2 U' F'", "U' L U' L' R U' R'", "U2 L F' U' F L'", "U' R' U R U R U R' U' R U2 R'", "U2 R' U2 R F' U' F", "U R U' R' U2 R U' R'", "R U R' F R' F' R", "U2 F U F2 U' F", "U2 L' F' U' F L"],
["U R U2 R' U' R U R'", "R' F R F' U' F' U' F", "U2 F R' F' R U R U R'", "U F' U2 F U F' U' F", "U R B U2 B' R'", "F' U2 F U' F' U F", "U2 R U2 R' U R U' R'", "U F' L' U2 L F", "U R2 D B2 D' R2", "U' R B' U B R'", "R' U R2 U' R'", "U B' R B2 U' B' U2 R'", "U2 R U' R' U R U' R' U R U' R'", "U R U' R' F' U2 F", "U2 F U' F2 U F", "U L F' L2 U L U2 F"],
["R U' R' U R U R'", "U R U' R' U2 F' U' F", "U R' U2 R2 U R2 U R", "F' U F", "R U R' U R U R'", "U2 F' U' F U2 F' U F", "U R U R'", "U L' B2 R2 B R2 B L", "L U L' R U R'", "U B' R U R' B", "U R' U' R2 U R'", "U F D R D' F'", "U2 R U R' U2 R U R'", "U2 F' U' F U' R U R'", "L' U' L R U' R'", "U F U2 F' R U R'"],
["U2 R U' R'", "U F U2 F2 U' F2 U' F'", "U F' U F U2 R U R'", "U2 F' U F U' F' U' F", "U B L2 F2 L' F2 L' B'", "U F' U' F", "R U R' U2 R U' R'", "U2 F' U' F U' F' U' F", "U2 L U' L' R U' R'", "U L F' U' F L'", "U2 R' U R U R U R' U' R U2 R'", "U R' U2 R F' U' F", "R U' R' U2 R U' R'", "U' R U R' F R' F' R", "U F U F2 U' F", "U L' F' U' F L"],
["U R U2 R' U R U' R'", "F' L' U2 L F", "R U2 R' U' R U R'", "U' R' F R F' U' F' U' F", "U F R' F' R U R U R'", "F' U2 F U F' U' F", "R B U2 B' R'", "U' F' U2 F U' F' U F", "R2 D B2 D' R2", "U2 R B' U B R'", "U' R' U R2 U' R'", "B' R B2 U' B' U2 R'", "U R U' R' U R U' R' U R U' R'", "R U' R' F' U2 F", "U F U' F2 U F", "L F' L2 U L U2 F"],
["R U R'", "L' B2 R2 B R2 B L", "U' R U' R' U R U R'", "R U' R' U2 F' U' F", "R' U2 R2 U R2 U R", "U' F' U F", "U' R U R' U R U R'", "U F' U' F U2 F' U F", "U' L U L' R U R'", "B' R U R' B", "R' U' R2 U R'", "F D R D' F'", "U R U R' U2 R U R'", "U F' U' F U' R U R'", "U' L' U' L R U' R'", "F U2 F' R U R'"],
["U' R U R' U2 R U' R'", "U F' U' F U' F' U' F", "U R U' R'", "F U2 F2 U' F2 U' F'", "F' U F U2 R U R'", "U F' U F U' F' U' F", "B L2 F2 L' F2 L' B'", "F' U' F", "U L U' L' R U' R'", "L F' U' F L'", "U R' U R U R U R' U' R U2 R'", "R' U2 R F' U' F", "U' R U' R' U2 R U' R'", "U2 R U R' F R' F' R", "F U F2 U' F", "L' F' U' F L"],
["U' R B U2 B' R'", "U2 F' U2 F U' F' U F", "R U2 R' U R U' R'", "U' F' L' U2 L F", "U' R U2 R' U' R U R'", "U2 R' F R F' U' F' U' F", "F R' F' R U R U R'", "U' F' U2 F U F' U' F", "U' R2 D B2 D' R2", "U R B' U B R'", "U2 R' U R2 U' R'", "U' B' R B2 U' B' U2 R'", "R U' R' U R U' R' U R U' R'", "U' R U' R' F' U2 F", "F U' F2 U F", "U' L F' L2 U L U2 F"],
["U2 R U R' U R U R'", "F' U' F U2 F' U F", "U' R U R'", "U' L' B2 R2 B R2 B L", "U2 R U' R' U R U R'", "U' R U' R' U2 F' U' F", "U' R' U2 R2 U R2 U R", "U2 F' U F", "U2 L U L' R U R'", "U' B' R U R' B", "U' R' U' R2 U R'", "U' F D R D' F'", "R U R' U2 R U R'", "F' U' F U' R U R'", "U2 L' U' L R U' R'", "U' F U2 F' R U R'"],
["U' B L2 F2 L' F2 L' B'", "U' F' U' F", "U2 R U R' U2 R U' R'", "F' U' F U' F' U' F", "R U' R'", "U' F U2 F2 U' F2 U' F'", "U' F' U F U2 R U R'", "F' U F U' F' U' F", "L U' L' R U' R'", "U' L F' U' F L'", "R' U R U R U R' U' R U2 R'", "U' R' U2 R F' U' F", "U2 R U' R' U2 R U' R'", "U R U R' F R' F' R", "U' F U F2 U' F", "U' L' F' U' F L"],
["U2 B' U B R U2 R'", "U L U' L' F' U2 F", "U B' U B R U2 R'", "L U' L' F' U2 F", "B' U B R U2 R'", "U' L U' L' F' U2 F", "U' B' U B R U2 R'", "U2 L U' L' F' U2 F", "L R U2 R' L'", "L U' L' U2 L' B2 R2 B R2 B L", "R' U R B' U B R U2 R'", "R' U R U' L U' L' F' U2 F", "F' U' F L U' L' F' U2 F", "R L U L' R' U' F' U F", "F U' F' L U' L' F' U2 F", "F U' F' U B' U B R U2 R'"],
["U' L U' L' R U2 R'", "L' B L B' R U2 R'", "U2 L U' L' R U2 R'", "U' L' B L B' R U2 R'", "U L U' L' R U2 R'", "U2 L' B L B' R U2 R'", "L U' L' R U2 R'", "U L' B L B' R U2 R'", "B' U B U F' U2 F U' F' U F", "B' U B U2 R B U2 B' R'", "R' U R U L U' L' R U2 R'", "R' U R U2 L' B L B' R U2 R'", "R U R' U L U' L' R U2 R'", "L U2 L' R U' R' F' U2 F", "L' U' L U L U' L' R U2 R'", "F U F' L U' L' R U2 R'"],
["U' B L' B' L F' U2 F", "B' U B F' U2 F", "U2 B L' B' L F' U2 F", "U' B' U B F' U2 F", "U B L' B' L F' U2 F", "U2 B' U B F' U2 F", "B L' B' L F' U2 F", "U B' U B F' U2 F", "L U' L' U' R U2 R' U R U' R'", "L U' L' U2 F' L' U2 L F", "R' U R U B L' B' L F' U2 F", "R' U R U2 B' U B F' U2 F", "R U R' U B L' B' L F' U2 F", "B' U2 B R U' R' F' U2 F", "L' U' L U B L' B' L F' U2 F", "L' U' L U2 B' U B F' U2 F"],
["U R' U2 R U' R U R'", "U2 B U' B' F' U F", "R' U2 R U' R U R'", "U B U' B' F' U F", "U' R' U2 R U' R U R'", "B U' B' F' U F", "U2 R' U2 R U' R U R'", "U' B U' B' F' U F", "L U2 L' R' U2 R U' R U R'", "L U' L' B U' B' F' U F", "R' U R2 U2 R' U' R U2 R'", "F R' F2 U F2 R F'", "F' U' F U B U' B' F' U F", "R' U' F R F' U2 R U R'", "F U' F' U B U' B' F' U F", "F U' F' R' U2 R U' R U R'"],
["R' U' R2 U2 R'", "U' B U' B' U F' U' F", "U' R' U' R2 U2 R'", "U2 B U' B' U F' U' F", "U2 R' U' R2 U2 R'", "U B U' B' U F' U' F", "U R' U' R2 U2 R'", "B U' B' U F' U' F", "L U L' R' U' R2 U2 R'", "L U' L' U B U' B' U F' U' F", "R' U R U R B U2 B' R'", "R' U R F' U2 F U' F' U F", "F' U F B U' B' U F' U' F", "B' R2 B R' U R'", "B U2 B' F U' F2 U F", "F U2 F' R' U' R2 U2 R'"],
["B U B' R U R'", "U B U B' U' F' U F", "U' B U B' R U R'", "B U B' U' F' U F", "U2 B U B' R U R'", "U' B U B' U' F' U F", "U B U B' R U R'", "U2 B U B' U' F' U F", "L U L' B U B' R U R'", "L U' L' U' B U B' U' F' U F", "R' U R2 U R' U R U R'", "R' U R U2 F' U' F U2 F' U F", "F' U' F B U B' U' F' U F", "F R' F' U R F' U F", "F U' F' B U B' U' F' U F", "F U2 F' B U B' R U R'"],
["R' F R F' R U R'", "U' R U' R' F R' F' R", "U' R' F R F' R U R'", "U2 R U' R' F R' F' R", "U2 R' F R F' R U R'", "U R U' R' F R' F' R", "U R' F R F' R U R'", "R U' R' F R' F' R", "L2 D F2 D' L2", "L D F' U' F D' L'", "R2 U' R2 U R2", "B' D' R D B", '', "R2 U2 F R2 F' U2 R' U R'", "F L F2 L' F'", "L D F' D' L'"],
["U R U' R' U R U' R'", "U2 F' U' F U F' U' F", "R U' R' U R U' R'", "U F' U' F U F' U' F", "U' R U' R' U R U' R'", "F' U' F U F' U' F", "U2 R U' R' U R U' R'", "U' F' U' F U F' U' F", "R U R' U' R2 D B2 D' R2", "R U2 B' U B R'", "F R' F2 U2 F R2 U2 R'", "F' U2 F R' U' R U' F' U' F", "R U' R' U' R U R' U2 R U' R'", "F' L' U2 L F R U R'", "R U R' F U' F2 U F", "R U' R2 D' F' D R"],
["U R U R' U' R U R'", "U2 F' U F U' F' U F", "R U R' U' R U R'", "U F' U F U' F' U F", "U' R U R' U' R U R'", "F' U F U' F' U F", "U2 R U R' U' R U R'", "U' F' U F U' F' U F", "B' U2 B R U2 R' F' U2 F", "F' U2 L U' L' F", "F' U' F R' U R2 U' R'", "F R' F' U R U' F' U F", "R U' R' U R U2 R' U R U' R'", "R U' R' F' L' U2 L F", "F U F' U' R U R' U2 F' U F", "F' U' F' U R U' R' F U2 F"],
["U L' U L R U' R'", "U2 F U2 F' U F' U' F", "L' U L R U' R'", "U F U2 F' U F' U' F", "U' L' U L R U' R'", "F U2 F' U F' U' F", "U2 L' U L R U' R'", "U' F U2 F' U F' U' F", "L' U2 L2 U L' R U R'", "L U' L' F U2 F' U F' U' F", "R' U R U' L' U L R U' R'", "R' U R F U2 F' U F' U' F", "R U R' U' L' U L R U' R'", "F U R' F' R U2 F' U' F", "F U' F2 U2 F U2 F' U F", "R' F R2 U' R2 F' R"],
["U2 L' U' L U R U' R'", "U' L' U' L F' U' F", "U L' U' L U R U' R'", "U2 L' U' L F' U' F", "L' U' L U R U' R'", "U L' U' L F' U' F", "U' L' U' L U R U' R'", "L' U' L F' U' F", "L' U' L U L U' L' R U' R'", "L' U' L2 F' U' F L'", "R' U R L' U' L U R U' R'", "R' U2 R L' U' L F' U' F", "R U R' L' U' L U R U' R'", "R' F R U' F' R U' R'", "F U' F2 U' F U' F' U' F", "L' U L U2 F' U2 F U' F' U F"],
["L' U L U' R U R'", "U' F U F2 U2 F", "U' L' U L U' R U R'", 'U2 F U F2 U2 F', "U2 L' U L U' R U R'", 'U F U F2 U2 F', "U L' U L U' R U R'", 'F U F2 U2 F', "L' U' L R2 D B2 D' R2", "L' U' L U2 R B' U B R'", "R' U' R L' U L U' R U R'", "R' U2 R F U F2 U2 F", "R U R' U2 L' U L U' R U R'", "L F2 L' F U' F", "L' U L U2 R U R' U R U R'", "F U' F' R U2 R' U R U' R'"]]


########################################  FUNCTIONS  ###########################################################
########################################  CUBE DATA INTERFACE
def npind(CI: tuple[int, int, int]) -> tuple[int, int, int]: # input is Cartesian Indexing. output is numpy indexing.
    x, y, z = CI
    return (x+2, y+2, z+2)
def CI_to_int(CI: tuple[int, int, int]) -> np.uint8:
    # number is uint8 11ZZYYXX, where XX, YY, and ZZ are x, y, and z each + 1
    return np.uint8(0b11000000 + ((CI[2]+1)<<4) + ((CI[1]+1)<<2) + (CI[0]+1))
def int_to_CI(INT: np.uint8) -> tuple[int, int, int]:
    mask = INT - 0b11000000
    x, y, z = int((mask&0b11))-1, int((mask&0b1111)>>2)-1, int((mask&0b111111)>>4)-1
    return (x, y, z)
######################################## CREATE CUBE
def cube3d_from_cube2ds(cube):
    # x, y, z point right, up, and back, respectively.
    cube3d = np.zeros(shape=(5, 5, 5), dtype=np.uint8)
    # U face
    for x in (-1, 0, 1):
        for z in (-1, 0, 1):
            cube3d[npind((x, 2, z))] = faces_to_nums[cube['U'][(1-z, x+1)]]
    # R face
    for z in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube3d[npind((2, y, z))] = faces_to_nums[cube['R'][(1-y, z+1)]]
    # F face
    for x in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube3d[npind((x, y, -2))] = faces_to_nums[cube['F'][(1-y, x+1)]]
    # L face
    for z in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube3d[npind((-2, y, z))] = faces_to_nums[cube['L'][(1-y, 1-z)]]
    # B face
    for x in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube3d[npind((x, y, 2))] = faces_to_nums[cube['B'][(1-y, 1-x)]]
    # D face
    for x in (-1, 0, 1):
        for z in (-1, 0, 1):
            cube3d[npind((x, -2, z))] = faces_to_nums[cube['D'][(1-z, 1-x)]]
    # corners
    for CI in ((-1, -1, -1), (-1, -1, 1), (-1, 1, -1), (-1, 1, 1), (1, -1, -1), (1, -1, 1), (-1, 1, -1), (1, 1, 1)):
        corner = {faces[cube3d[npind((2*CI[0], CI[1], CI[2]))]],
                  faces[cube3d[npind((CI[0], 2*CI[1], CI[2]))]],
                  faces[cube3d[npind((CI[0], CI[1], 2*CI[2]))]]}
        
        if corner == {'R', 'F', 'U'}: cube3d[npind(CI)] = CI_to_int((1, 1, -1))
        if corner == {'F', 'L', 'U'}: cube3d[npind(CI)] = CI_to_int((-1, 1, -1))
        if corner == {'L', 'B', 'U'}: cube3d[npind(CI)] = CI_to_int((-1, 1, 1))
        if corner == {'B', 'R', 'U'}: cube3d[npind(CI)] = CI_to_int((1, 1, 1))

        if corner == {'R', 'B', 'D'}: cube3d[npind(CI)] = CI_to_int((1, -1, 1))
        if corner == {'F', 'R', 'D'}: cube3d[npind(CI)] = CI_to_int((1, -1, -1))
        if corner == {'L', 'F', 'D'}: cube3d[npind(CI)] = CI_to_int((-1, -1, -1))
        if corner == {'B', 'L', 'D'}: cube3d[npind(CI)] = CI_to_int((-1, -1, 1))
    # edges
    for CI in ((-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0),
               (-1, 0, -1), (-1, 0, 1), (1, 0, -1), (1, 0, 1),
               (0, -1, -1), (0, -1, 1), (0, 1, -1), (0, 1, 1)):
        if CI[0]==0: edge = {faces[cube3d[npind((0, 2*CI[1], CI[2]))]], faces[cube3d[npind((0, CI[1], 2*CI[2]))]]}
        if CI[1]==0: edge = {faces[cube3d[npind((2*CI[0], 0, CI[2]))]], faces[cube3d[npind((CI[0], 0, 2*CI[2]))]]}
        if CI[2]==0: edge = {faces[cube3d[npind((2*CI[0], CI[1], 0))]], faces[cube3d[npind((CI[0], 2*CI[1], 0))]]}

        if edge == {'R', 'U'}: cube3d[npind(CI)] = CI_to_int((1, 1, 0))
        if edge == {'F', 'U'}: cube3d[npind(CI)] = CI_to_int((0, 1, -1))
        if edge == {'L', 'U'}: cube3d[npind(CI)] = CI_to_int((-1, 1, 0))
        if edge == {'B', 'U'}: cube3d[npind(CI)] = CI_to_int((0, 1, 1))

        if edge == {'R', 'F'}: cube3d[npind(CI)] = CI_to_int((1, 0, -1))
        if edge == {'F', 'L'}: cube3d[npind(CI)] = CI_to_int((-1, 0, -1))
        if edge == {'L', 'B'}: cube3d[npind(CI)] = CI_to_int((-1, 0, 1))
        if edge == {'B', 'R'}: cube3d[npind(CI)] = CI_to_int((1, 0, 1))

        if edge == {'R', 'D'}: cube3d[npind(CI)] = CI_to_int((1, -1, 0))
        if edge == {'F', 'D'}: cube3d[npind(CI)] = CI_to_int((0, -1, -1))
        if edge == {'L', 'D'}: cube3d[npind(CI)] = CI_to_int((-1, -1, 0))
        if edge == {'B', 'D'}: cube3d[npind(CI)] = CI_to_int((0, -1, 1))
    # centers
    for CI in ((1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1)):
        cube3d[npind(CI)] = CI_to_int(CI)
    return cube3d
def CUBE3D_from_scramble(scramble):
    cube2ds =   {'U': np.array([['U', 'U', 'U'], 
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
                                ['D', 'D', 'D']])}
    cube3d = cube3d_from_cube2ds(cube2ds)
    for move in scramble.split():
        update_cube_with_F_move(cube3d, move)
    return cube3d
######################################## DEBUGGING
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

    for x in (-1, 0, 1):
        for z in (-1, 0, 1):
            cube2ds['U'][(1-z, x+1)] = faces[cube3d[npind((x, 2, z))]]
    for z in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube2ds['R'][(1-y, z+1)] = faces[cube3d[npind((2, y, z))]]
    for x in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube2ds['F'][(1-y, x+1)] = faces[cube3d[npind((x, y, -2))]]
    for z in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube2ds['L'][(1-y, 1-z)] = faces[cube3d[npind((-2, y, z))]]
    for x in (-1, 0, 1):
        for y in (-1, 0, 1):
            cube2ds['B'][(1-y, 1-x)] = faces[cube3d[npind((x, y, 2))]]
    for x in (-1, 0, 1):
        for z in (-1, 0, 1):
            cube2ds['D'][(1-z, 1-x)] = faces[cube3d[npind((x, -2, z))]]
    return cube2ds
def printcube(cube3d):
    cube = cube2ds_from_cube3d(cube3d)
    for key, value in cube.items():
        print(f'{key}: ')
        print(value)
    print('-------------------------------------')
######################################## CUBE and ALG MANIPULATION
def update_cube_with_F_move(cube3d, F_move: str) -> None:
    Fmove = F_move[0]
    mod = (F_move[-1]*(len(F_move)==2))
    cube3dnew = np.copy(cube3d)
    ITERS = ((-2, -1, 0, 1, 2), (1, 2), (-2, -1))
    xiters = {'U': 0, 'R': 1, 'F': 0, 'L': 2, 'B': 0, 'D': 0}
    yiters = {'U': 1, 'R': 0, 'F': 0, 'L': 0, 'B': 0, 'D': 2}
    ziters = {'U': 0, 'R': 0, 'F': 2, 'L': 0, 'B': 1, 'D': 0}
    xiter, yiter, ziter = ITERS[xiters[Fmove]], ITERS[yiters[Fmove]], ITERS[ziters[Fmove]]
    if (Fmove, mod) in (('U', ""), ('D', "'")): rotation = lambda x, y, z: (z, y, -x)
    if (Fmove, mod) in (('U', "'"), ('D', "")): rotation = lambda x, y, z: (-z, y, x)
    if (Fmove, mod) in (('U', "2"), ('D', "2")): rotation = lambda x, y, z: (-x, y, -z)
    if (Fmove, mod) in (('R', ""), ('L', "'")): rotation = lambda x, y, z: (x, -z, y)
    if (Fmove, mod) in (('R', "'"), ('L', "")): rotation = lambda x, y, z: (x, z, -y)
    if (Fmove, mod) in (('R', "2"), ('L', "2")): rotation = lambda x, y, z: (x, -y, -z)
    if (Fmove, mod) in (('F', ""), ('B', "'")): rotation = lambda x, y, z: (y, -x, z)
    if (Fmove, mod) in (('F', "'"), ('B', "")): rotation = lambda x, y, z: (-y, x, z)
    if (Fmove, mod) in (('F', "2"), ('B', "2")): rotation = lambda x, y, z: (-x, -y, z)
    for x in xiter:
        for y in yiter:
            for z in ziter:
                cube3dnew[npind(rotation(x, y, z))] = cube3d[npind((x, y, z))]
    cube3d[:] = cube3dnew
def algorithm_from_Fs_perspective(face, algorithm):
    return [f'{faces[move_from_Fs_perspective[(faces_to_nums[face], faces_to_nums[move[0]])]]}{(move[-1]*(len(move)==2))}'
            for move in [i.strip(",") for i in algorithm.split()]]
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
######################################## CUBE SOLVING
################# C
def find_misplaced_bottom_edges(cube3d) -> tuple[str, int, int]:
    lost_souls = []
    for x in (-2, -1, 0, 1, 2):
        for y in (-2, -1, 0, 1, 2):
            for z in (-2, -1, 0, 1, 2):
                # make sure it is a bottom edge
                if cube3d[npind((x, y, z))] == faces_to_nums['D'] and x*y*z == 0:
                    # make sure it is wrong
                    if y != -2 or int_to_CI(cube3d[npind((x, -1, z))]) != (x, -1, z):
                        lost_souls.append((x, y, z))
    return lost_souls
def get_alg_for_bottom_edge(cube3d, coords: tuple[int, int, int]) -> None:
    x, y, z = coords
    if abs(x) == 2: destination = int_to_CI(cube3d[npind((x//2, y, z))])
    if abs(y) == 2: destination = int_to_CI(cube3d[npind((x, y//2, z))])
    if abs(z) == 2: destination = int_to_CI(cube3d[npind((x, y, z//2))])
    if destination == (1, -1, 0): newxyz, dest_face = (z, y, -x), 'R'
    if destination == (0, -1, -1): newxyz, dest_face = (x, y, z), 'F'
    if destination == (-1, -1, 0): newxyz, dest_face = (-z, y, x), 'L'
    if destination == (0, -1, 1): newxyz, dest_face = (-x, y, -z), 'B'
    return algorithm_from_Fs_perspective(dest_face, bottom_cross_algorithms[newxyz])
################# F
def Find_misplaced_f2l_pairs(cube3d):
    # All f2l_pairs: ('F', 'R'), ('L', 'F'), ('B', 'L'), ('R', 'B')
    corners = {('F', 'R'): (), 
               ('L', 'F'): (), 
               ('B', 'L'): (), 
               ('R', 'B'): ()}
    edges = {('F', 'R'): (), 
            ('L', 'F'): (), 
            ('B', 'L'): (), 
            ('R', 'B'): ()}
    for x in (-1, 0, 1):
        for y in (-1, 0, 1):
            for z in (-1, 0, 1):
                if (x, y, z) == (0, 0, 0): continue
                s = tuple({(2*x, y, z), (x, 2*y, z), (x, y, 2*z)}.difference({(x, y, z)}))
                s2 = tuple(faces[cube3d[npind(i)]] for i in s)
                if len(s)==2 and 'D' not in s2 and 'U' not in s2:
                    for i in edges.keys():
                        if i[0] in s2 and i[1] in s2:
                            edges[i] = s[s2.index(i[0])]
                if len(s)==3 and 'U' not in s2:
                    for i in corners.keys():
                        if i[0] in s2 and i[1] in s2:
                            corners[i] = s[s2.index(i[0])]
    f2l_pairs = []
    if corners[('F', 'R')] != (1, -1, -2) or edges[('F', 'R')] != (1, 0, -2): f2l_pairs.append((corners[('F', 'R')], edges[('F', 'R')], 'F'))
    if corners[('L', 'F')] != (-2, -1, -1) or edges[('L', 'F')] != (-2, 0, -1): f2l_pairs.append((corners[('L', 'F')], edges[('L', 'F')], 'L'))
    if corners[('B', 'L')] != (-1, -1, 2) or edges[('B', 'L')] != (-1, 0, 2): f2l_pairs.append((corners[('B', 'L')], edges[('B', 'L')], 'B'))
    if corners[('R', 'B')] != (2, -1, 1) or edges[('R', 'B')] != (2, 0, 1): f2l_pairs.append((corners[('R', 'B')], edges[('R', 'B')], 'R'))
    return f2l_pairs
def get_alg_for_f2l_pair(f2l_pair):
    corner, edge, face = f2l_pair
    #print(f2l_pair)
    if face == 'R': transformation = lambda x, y, z: (z, y, -x)
    if face == 'F': transformation = lambda x, y, z: (x, y, z)
    if face == 'L': transformation = lambda x, y, z: (-z, y, x)
    if face == 'B': transformation = lambda x, y, z: (-x, y, -z)
    newxyzC = transformation(corner[0], corner[1], corner[2])
    newxyzE = transformation(edge[0], edge[1], edge[2])
    return algorithm_from_Fs_perspective(face, F2L_ALGORITHMS[corners_to_indices[newxyzC]][edges_to_indices[newxyzE]])
################# O
def get_OLL_algorithm(cube3d):
    FULL_OLL = {0b010100011010110001010: "R U2 R2 F R F' U2 R' F R F'",
                0b111000001010110001010: "L F L' U2 L F2 R' F2 R F' L'",
                0b110000011010101000011: "L' R2 B R' B L U2 L' B L R'",
                0b011100001010100010110: "L' R B' L U2 L' B' R B' R2 L",
                0b000011010110110000011: "R' F2 L F L' F R",
                0b000101101011000001110: "L F2 R' F' R F' L'",
                0b100001010110101000011: "L F R' F R F2 L'",
                0b001101001011000010110: "R' F' L F' L' F2 R",
                0b001101000110100010110: "R U R' U' R' F R2 U R' U' F'",
                0b110000100110110100001: "R U R' U R' F R F' R U2 R'",
                0b100001100110110000011: "L F R' F R' D R D' R F2 L'",
                0b001011001011000001110: "L R2 F' R F' R' F2 R F' R L'",
                0b110000010111001000011: "L F' L' U' L F L' F' U F",
                0b011100000111000010110: "R' F R U R' F' R F U' F'",
                0b010010010111010000011: "R' F' R L' U' L U R' F R",
                0b010100100111000001110: "L F L' R U R' U' L F' L'",
                0b010010011010100010110: "F R' F' R2 L' B R B' R' B' L R'",
                0b010010101010100000111: "L F R' F R F2 L2 B' R B' R' B2 L",
                0b010010101010110001010: "L' R B R B R' B' L R2 F R F'",
                0b010010101010101010010: "L F R' F' L2 R2 B R B' R' B' L R'",
                0b101001000111000100101: "R U2 R' U' R U R' U' R U' R'",
                0b001101000111010100001: "R U2 R2 U' R2 U' R2 U2 R",
                0b101001000111001110000: "R2 D' R U2 R' D R U2 R",
                0b100001100111000110100: "L F R' F' L' F R F'",
                0b000101100111001100001: "F' L F R' F' L' F R",
                0b000101100111000101100: "R U2 R' U' R U' R'",
                0b100001010111001100001: "R U R' U R U2 R'",
                0b000011100110101010010: "L F R' F' L' R U R U' R'",
                0b100001100110100010110: "R U R' U' R U' R' F' U' F R U R'",
                0b000101010110101010010: "F R' F R2 U' R' U' R U R' F2",
                0b100001101011000010110: "R' U' F U R U' R' F' R",
                0b001011000110101000011: "L U F' U' L' U L F L'",
                0b110000100111000010110: "R U R' U' R' F R F'",
                0b010100010111001010010: "R U R' U' B' R' F R F' B",
                0b010010011011000110100: "R U2 R2 F R F' R U2 R'",
                0b001011001011010010010: "L' U' L U' L' U L U L F' L' F",
                0b000011010110100010110: "F R' F' R U R U' R'",
                0b100001100110101001010: "R U R' U R U' R' U' R' F R F'",
                0b110000100111001001010: "L F' L' U' L U F U' L'",
                0b011010000111010010010: "R' F R U R' U' F' U R",
                0b101001000110101010010: "R U R' U R U2 R' F R U R' U' F'",
                0b010010100110100100101: "R' U' R U' R' U2 R F R U R' U' F'",
                0b000101101011010010010: "F' U' L' U L F",
                0b000011010110101001010: "F U R U' R' F'",
                0b010100100111010010010: "F R U R' U' F'",
                0b000011011010101101000: "R' U' R' F R F' U R",
                0b100001011011000001110: "F' L' U' L U L' U' L U F",
                0b001101000110110000011: "F R U R' U' R U R' U' F'",
                0b001101001011010000011: "L F' L2 B L2 F L2 B' L",
                0b011100001011010100001: "L' B L2 F' L2 B' L2 F L'",
                0b110000010111000001110: "F U R U' R' U R U' R' F'",
                0b100001011010100101100: "R U R' U R U' B U' B' R'",
                0b101001001011000000111: "R' F2 L F L' F' L F L' F R",
                0b101001000110100000111: "L F2 R' F' R F R' F' R F' L'",
                0b000101011010110101000: "R U2 R2 U' R U' R' U2 F R F'",
                0b010100010111010001010: "L' B' L U' R' U R U' R' U R L' B L",
                0b010010100111001010010: "R U R' U' L R' F R F' L'",
                0b000011100111001110000: ""}
    rotation0 = lambda x, y, z: (-z, y, x)  # U', 'R'
    rotation1 = lambda x, y, z: (x, y, z)   #  , 'F'
    rotation2 = lambda x, y, z: (z, y, -x)  # U, 'L'
    rotation3 = lambda x, y, z: (-x, y, -z) # U2, 'B'
    top_layer_num0 = 0
    top_layer_num1 = 0
    top_layer_num2 = 0
    top_layer_num3 = 0
    for x, y, z in ((-1, 1, 2), (0, 1, 2), (1, 1, 2), 
                (-2, 1, 1), (-1, 2, 1), (0, 2, 1), (1, 2, 1), (2, 1, 1),
                (-2, 1, 0), (-1, 2, 0), (0, 2, 0), (1, 2, 0), (2, 1, 0),
                (-2, 1, -1), (-1, 2, -1), (0, 2, -1), (1, 2, -1), (2, 1, -1),
                (-1, 1, -2), (0, 1, -2), (1, 1, -2)):
        top_layer_num0 <<= 1
        top_layer_num1 <<= 1
        top_layer_num2 <<= 1
        top_layer_num3 <<= 1
        top_layer_num0 |= (cube3d[npind(rotation0(x, y, z))]==0)
        top_layer_num1 |= (cube3d[npind(rotation1(x, y, z))]==0)
        top_layer_num2 |= (cube3d[npind(rotation2(x, y, z))]==0)
        top_layer_num3 |= (cube3d[npind(rotation3(x, y, z))]==0)
    #print(top_layer_num)
    if top_layer_num0 in FULL_OLL:
        return algorithm_from_Fs_perspective('R', FULL_OLL[top_layer_num0])
    if top_layer_num1 in FULL_OLL:
        return algorithm_from_Fs_perspective('F', FULL_OLL[top_layer_num1])
    if top_layer_num2 in FULL_OLL:
        return algorithm_from_Fs_perspective('L', FULL_OLL[top_layer_num2])
    if top_layer_num3 in FULL_OLL:
        return algorithm_from_Fs_perspective('B', FULL_OLL[top_layer_num3])
################# P
def get_PLL_algorithm(cube3d):
    #printcube(cube3d)
    FULL_PLL = {"RLRFBFLRLBFB": "L2 R2 D L2 R2 U2 L2 R2 D L2 R2",                 # good
                "RLRFRFLFLBBB": "R U' R U R U R U' R' U' R2",                     # good
                "RFRFLFLRLBBB": "R2 U R U R' U' R' U' R' U R'",                   # good
                "FRFLBLBLBRFR": "L R' F L2 R2 B L2 R2 F L R' D2 L2 R2",           # good
                "FRBRFLBLFLBR": "R B' R' F R B R' F' R B R' F R B' R' F'",        # good
                "FFBRRLBBFLLR": "F U' B U2 F' U B' F U' B U2 F' U B'",            # good
                "BFFLRRFBBRLL": "L' U R' U2 L U' R L' U R' U2 L U' R",            # good
                "RBLBFFLLRFRB": "R' U R' U' B' R' B2 U' B' U B' R B R",           # good
                "RRLBFFLBRFLB": "F R' F R2 U' R' U' R U R' F' R U R' U' F'",      # good
                "LRRFFLBLBRBF": "L2 B2 L' F' L B2 L' F L'",                       # good
                "LRBRFRFLLBBF": "R B' R F2 R' B R F2 R2",                         # good
                "FRBRBFLLLBFR": "R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R", # good
                "RRFLBLBFRFLB": "B2 D L' U L' U' L D' B2 R' U R",                 # good
                "BLRFFBRBFLRL": "F' U' F R2 D B' U B U' B D' R2",                 # good
                "BRRFLBRBFLFL": "F2 D' L U' L U L' D F2 R U' R'",                 # good
                "RLFLRLBFRFBB": "B U B' R2 D' F U' F' U F' D R2",                 # good
                "FBBRFFLLLBRR": "R2 D R D' R F2 L' U L F2",                       # good
                "LLRFFLBBBRRF": "R U R' F' R U R' U' R' F R2 U' R'",              # good
                "BRBRBFLFRFLL": "F U2 F' U2 F R' F' U' F U F R F2",               # good
                "RBLBFRFRFLLB": "R2 F R U R U' R' F' R U2 R' U2 R",               # good
                "FLBRFFLRLBBR": "R U R' U' R' F R2 U' R' U' R U R' F'",           # good
                "RRRFFFLLLBBB": ""}                                               # good
    cube3dnew = np.copy(cube3d)
    for v in [[], ['U'], ['U2'], ["U'"]]:
        pllF = ''.join([faces[cube3dnew[npind(coord)]] 
                        for coord in ((2, 1, 1), (2, 1, 0), (2, 1, -1),
                                      (1, 1, -2), (0, 1, -2), (-1, 1, -2),
                                      (-2, 1, -1), (-2, 1, 0), (-2, 1, 1),
                                      (-1, 1, 2), (0, 1, 2), (1, 1, 2))])
        pllL = ''.join(["RFLB"[("RFLB".find(i)-1)%4] for i in pllF[3:]+pllF[:3]])
        pllB = ''.join(["RFLB"[("RFLB".find(i)-1)%4] for i in pllL[3:]+pllL[:3]])
        pllR = ''.join(["RFLB"[("RFLB".find(i)-1)%4] for i in pllB[3:]+pllB[:3]])
        if pllF in FULL_PLL: return v + algorithm_from_Fs_perspective('F', FULL_PLL[pllF])
        if pllL in FULL_PLL: return v + algorithm_from_Fs_perspective('L', FULL_PLL[pllL])
        if pllB in FULL_PLL: return v + algorithm_from_Fs_perspective('B', FULL_PLL[pllB])
        if pllR in FULL_PLL: return v + algorithm_from_Fs_perspective('R', FULL_PLL[pllR])
        update_cube_with_F_move(cube3dnew, 'U')
######################################## RECURSIVE SOLVING
def OP(F_moves, cube3d): # At the end of Recursive_F, OP is called, thereby completing the recursive CFOP
    cube3dnew = np.copy(cube3d)
    algorithmOLL = get_OLL_algorithm(cube3dnew)
    for move in algorithmOLL: update_cube_with_F_move(cube3dnew, move)
    algorithmPLL = get_PLL_algorithm(cube3dnew)
    for move in algorithmPLL: update_cube_with_F_move(cube3dnew, move)
    new_F_moves = simplify_FURDLB_MOVES(F_moves + algorithmOLL + algorithmPLL)
    return (new_F_moves, cube3dnew)
def Recursive_F(F_moves, cube3d): # At the end of Recursive_C, Recursive_F is called
    lost_pairs = Find_misplaced_f2l_pairs(cube3d)
    best = None
    if not lost_pairs: 
        #print(simplify_FURDLB_MOVES(F_moves))
        return OP(F_moves, cube3d)
    for pair in lost_pairs:
        cube3dnew = np.copy(cube3d)
        algorithm = get_alg_for_f2l_pair(pair)
        for move in algorithm: update_cube_with_F_move(cube3dnew, move)
        candidate = Recursive_F(F_moves + algorithm, cube3dnew)
        if best is None or len(candidate[0]) < len(best[0]):
            best = candidate
    return best
def Recursive_C(F_moves, cube3d):
    lost_edges = find_misplaced_bottom_edges(cube3d)
    best = None
    if not lost_edges: 
        #print(simplify_FURDLB_MOVES(F_moves))
        return Recursive_F(F_moves, cube3d)
    for edge in lost_edges:
        cube3dnew = np.copy(cube3d)
        algorithm = get_alg_for_bottom_edge(cube3d, edge)
        for move in algorithm: update_cube_with_F_move(cube3dnew, move)
        candidate = Recursive_C(F_moves + algorithm, cube3dnew)
        if best is None or len(candidate[0]) < len(best[0]):
            best = candidate
    return best
######################################### RUN #########################################################################
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
    #CUBE3D = cube3d_from_cube2ds(CUBE)
    CUBE3D = CUBE3D_from_scramble(scramble)
    F_MOVES, CUBE3D = Recursive_C([], CUBE3D)
    #printcube(CUBE3D)
    #print(F_MOVES)
    print(f'move count: {len(F_MOVES)}')
    move_counts.append(len(F_MOVES))
    t = time.perf_counter() - start
    print(f"{t} s")

print(sum(move_counts)/len(move_counts))
