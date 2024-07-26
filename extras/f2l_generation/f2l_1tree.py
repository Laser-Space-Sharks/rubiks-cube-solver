import numpy as np
import time
corners_to_indices = {('L', 'B', 'U'): 0,   # const lookup table (the ordering can be changed, but the spreadsheet would have to change too)
                      ('B', 'U', 'L'): 1,
                      ('U', 'L', 'B'): 2,
                      ('B', 'R', 'U'): 3,
                      ('R', 'U', 'B'): 4,
                      ('U', 'B', 'R'): 5,
                      ('R', 'F', 'U'): 6,
                      ('F', 'U', 'R'): 7,
                      ('U', 'R', 'F'): 8,
                      ('F', 'L', 'U'): 9,
                      ('L', 'U', 'F'): 10,
                      ('U', 'F', 'L'): 11,
                      ('B', 'L', 'D'): 12,
                      ('L', 'D', 'B'): 13,
                      ('D', 'B', 'L'): 14,
                      ('R', 'B', 'D'): 15,
                      ('B', 'D', 'R'): 16,
                      ('D', 'R', 'B'): 17,
                      ('F', 'R', 'D'): 18,
                      ('R', 'D', 'F'): 19,
                      ('D', 'F', 'R'): 20,
                      ('L', 'F', 'D'): 21,
                      ('F', 'D', 'L'): 22,
                      ('D', 'L', 'F'): 23}
edges_to_indices = {('U', 'B'): 0,           # const lookup table (the ordering can be changed, but the spreadsheet would have to change too)
                   ('B', 'U'): 1,
                   ('U', 'R'): 2,
                   ('R', 'U'): 3,
                   ('U', 'F'): 4,
                   ('F', 'U'): 5,
                   ('U', 'L'): 6,
                   ('L', 'U'): 7,
                   ('B', 'L'): 8,
                   ('L', 'B'): 9,
                   ('B', 'R'): 10,
                   ('R', 'B'): 11,
                   ('F', 'R'): 12,
                   ('R', 'F'): 13,
                   ('F', 'L'): 14,
                   ('L', 'F'): 15}
F2L_MOVECOUNTS = np.ones(shape=(24, 16))*-1
F2L_ALGORITHMS = [[[] for _ in range(16)] for _ in range(24)] # 2d array of vectors[str]
F2L_MOVECOUNTS[corners_to_indices[('F', 'R', 'D')], edges_to_indices[('F', 'R')]] = 0
F2L_ALGORITHMS[corners_to_indices[('F', 'R', 'D')]][edges_to_indices[('F', 'R')]] = ['']

def is_valid_case(cube3d) -> bool:
    # the faces are numbers using URFLBD (our universal acronym and ordering to map faces to numbers)
       # properties of URFLBD include 
       #    - U, R, and F (the most common 3 moves) being first
       #    - the middle 4 faces (RFLB) being the sides in rotational order, 
       #    - the acronym spatially moving top to bottom.
       # The hope with a universal acronym was originally was to expose patterns that otherwise might have proven using group theory.
                
                #  (x, y, z, face)
    bottom_cross = ((1, -2, 0, 5), (0, -2, -1, 5), (-1, -2, 0, 5), (0, -2, 1, 5), 
                (2, -1, 0, 1), (0, -1, -2, 2), (-2, -1, 0, 3), (0, -1, 2, 4))
    # If the bottom cross is not preserved after applying the algorithm to a solved cube, then the result is not a valid f2l case.
    for a1, a2, a3, a4 in bottom_cross:
        if cube3d[a1+2, a2+2, a3+2] != a4:
            return False

    # These are to check if the 3 other slots were preserved (handling the possibility that the corner or edge is in the wrong slot)
    npind = lambda x, y, z: (x+2, y+2, z+2)
    ########################### BR ####################################################
    edge1_misplaced = not ((cube3d[npind(2, 0, 1)], cube3d[npind(1, 0, 2)]) == (1, 4))
    FRedge_in_edge1 = ({cube3d[npind(2, 0, 1)], cube3d[npind(1, 0, 2)]} == {1, 2})

    corner1_misplaced = not ((cube3d[npind(2, -1, 1)], cube3d[npind(1, -1, 2)], cube3d[npind(1, -2, 1)]) == (1, 4, 5))  # order does matter
    FRcorner_in_corner1 = ({cube3d[npind(2, -1, 1)], cube3d[npind(1, -1, 2)], cube3d[npind(1, -2, 1)]} == {1, 2, 5}) # order doesn't matter

    if (edge1_misplaced or corner1_misplaced) and not (FRedge_in_edge1 or FRcorner_in_corner1):
        return False
    ######################### LB #####################################################
    edge2_misplaced = not ((cube3d[npind(-1, 0, 2)], cube3d[npind(-2, 0, 1)]) == (4, 3))
    FRedge_in_edge2 = ({cube3d[npind(-1, 0, 2)], cube3d[npind(-2, 0, 1)]} == {1, 2})

    corner2_misplaced = not ((cube3d[npind(-1, -1, 2)], cube3d[npind(-2, -1, 1)], cube3d[npind(-1, -2, 1)]) == (4, 3, 5))  # order does matter
    FRcorner_in_corner2 = ({cube3d[npind(-1, -1, 2)], cube3d[npind(-2, -1, 1)], cube3d[npind(-1, -2, 1)]} == {1, 2, 5})  # order doesn't matter

    if (edge2_misplaced or corner2_misplaced) and not (FRedge_in_edge2 or FRcorner_in_corner2):
        return False
    ######################### FL #####################################################
    edge3_misplaced = not ((cube3d[npind(-2, 0, -1)], cube3d[npind(-1, 0, -2)]) == (3, 2))
    FRedge_in_edge3 = ({cube3d[npind(-2, 0, -1)], cube3d[npind(-1, 0, -2)]} == {1, 2})

    corner3_misplaced = not ((cube3d[npind(-2, -1, -1)], cube3d[npind(-1, -1, -2)], cube3d[npind(-1, -2, -1)]) == (3, 2, 5)) # order does matter
    FRcorner_in_corner3 = ({cube3d[npind(-2, -1, -1)], cube3d[npind(-1, -1, -2)], cube3d[npind(-1, -2, -1)]} == {1, 2, 5})  # order doesn't matter

    if (edge3_misplaced or corner3_misplaced) and not (FRedge_in_edge3 or FRcorner_in_corner3):
        return False
    ##################################################################################
    return True

                            #          corner              edge
def detect_case(cube3d) -> tuple[tuple[str, str, str], tuple[str, str]]:
    npind1 = lambda x: (x[0]+2, x[1]+2, x[2]+2)
    corner = None    # would declare as a 3-tuple
    edge = None      # would declare as a 2-tuple
    def getface(a):
        if a[1] == 2: return 'U'
        if a[0] == 2: return 'R'
        if a[2] == -2: return 'F'
        if a[0] == -2: return 'L'
        if a[2] == 2: return 'B'
        if a[1] == -2: return 'D'
    #################### CORNER #####################
    for x0, y0, z0 in ((-1, -1, -1), (-1, -1, 1), (-1, 1, -1), (-1, 1, 1), (1, -1, -1), (1, -1, 1), (1, 1, -1), (1, 1, 1)):
        xyzs = ((2*x0, y0, z0), (x0, 2*y0, z0), (x0, y0, 2*z0))
        e = (cube3d[npind1(xyzs[0])], cube3d[npind1(xyzs[1])], cube3d[npind1(xyzs[2])])
        #   shorthand for asking if (2 in e) and (1 in e) and (5 in e)
        if set(e) == {2, 1, 5}:      #     F                          R                          D
            corner = (getface(xyzs[e.index(2)]), getface(xyzs[e.index(1)]), getface(xyzs[e.index(5)]))
    ################### EDGE ########################
    for x0, y0, z0 in ((0, -1, -1), (0, -1, 1), (0, 1, -1), (0, 1, 1)):
        xyzs = ((0, 2*y0, z0), (0, y0, 2*z0))
        e = (cube3d[npind1(xyzs[0])], cube3d[npind1(xyzs[1])])
        #   shorthand for asking if (2 in e) and (1 in e)
        if set(e) == {1, 2}: #           F                          R
            edge = (getface(xyzs[e.index(2)]), getface(xyzs[e.index(1)]))
    for x0, y0, z0 in ((-1, 0, -1), (-1, 0, 1), (1, 0, -1), (1, 0, 1)):
        xyzs = ((2*x0, 0, z0), (x0, 0, 2*z0))
        e = (cube3d[npind1(xyzs[0])], cube3d[npind1(xyzs[1])])
        #   shorthand for asking if (2 in e) and (1 in e)
        if set(e) == {1, 2}: #           F                          R
            edge = (getface(xyzs[e.index(2)]), getface(xyzs[e.index(1)]))
    for x0, y0, z0 in ((-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0)):
        xyzs = ((2*x0, y0, 0), (x0, 2*y0, 0))
        e = (cube3d[npind1(xyzs[0])], cube3d[npind1(xyzs[1])])
        #   shorthand for asking if (2 in e) and (1 in e)
        if set(e) == {1, 2}: #           F                          R
            edge = (getface(xyzs[e.index(2)]), getface(xyzs[e.index(1)]))
    ################################################
    return (corner, edge)
                                                            #  (face, count)
def DFS_1tree_UDRLFB0(cube3d, alg_length: int, alg_so_far: list[tuple[int, int]]):
    for b in (1, -1, 2):
        cube3dnew = np.copy(cube3d)
        cube3dnew[:,3:,:] = np.rot90(cube3d, k=b, axes=(2, 0))[:,3:,:] # execute U move
        DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(0, b)])
        if len(alg_so_far) <= alg_length-2: 
            for b2 in (1, -1, 2):
                cube3dnewnew = np.copy(cube3dnew)
                cube3dnewnew[:,:2,:] = np.rot90(cube3dnew, k=b2, axes=(0, 2))[:,:2,:] # execute opposite of U move or otherwise D
                DFS_1tree_UDRLFB(cube3dnewnew, alg_length, alg_so_far + [(0, b), (5, b2)])
    # R
    for b in (1, -1, 2):
        cube3dnew = np.copy(cube3d)
        cube3dnew[3:,:,:] = np.rot90(cube3d, k=b, axes=(1, 2))[3:,:,:] # execute R move
        DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(1, b)])
        if len(alg_so_far) <= alg_length-2:
            for b2 in (1, -1, 2):
                cube3dnewnew = np.copy(cube3dnew)
                cube3dnewnew[:2,:,:] = np.rot90(cube3dnew, k=b2, axes=(2, 1))[:2,:,:] # execute opposite of R move or otherwise L
                DFS_1tree_UDRLFB(cube3dnewnew, alg_length, alg_so_far + [(1, b), (3, b2)])
    # F
    for b in (1, -1, 2):
        cube3dnew = np.copy(cube3d)
        cube3dnew[:,:,:2] = np.rot90(cube3d, k=b, axes=(1, 0))[:,:,:2] # execute F move
        DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(2, b)])
        if len(alg_so_far) <= alg_length-2:
            for b2 in (1, -1, 2):
                cube3dnewnew = np.copy(cube3dnew)
                cube3dnewnew[:,:,3:] = np.rot90(cube3dnew, k=b2, axes=(0, 1))[:,:,3:] # execute opposite of F move or otherwise B
                DFS_1tree_UDRLFB(cube3dnewnew, alg_length, alg_so_far + [(2, b), (4, b2)])
    # L
    for b in (1, -1, 2):
        cube3dnew = np.copy(cube3d)
        cube3dnew[:2,:,:] = np.rot90(cube3d, k=b, axes=(2, 1))[:2,:,:] # execute L move
        DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(3, b)])
    # B
    for b in (1, -1, 2):
        cube3dnew = np.copy(cube3d)
        cube3dnew[:,:,3:] = np.rot90(cube3d, k=b, axes=(0, 1))[:,:,3:] # execute B move
        DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(4, b)])
    # D
    for b in (1, -1, 2):
        cube3dnew = np.copy(cube3d)
        cube3dnew[:,:2,:] = np.rot90(cube3d, k=b, axes=(0, 2))[:,:2,:] # execute D move
        DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(5, b)])
def DFS_1tree_UDRLFB(cube3d, alg_length: int, alg_so_far: list[tuple[int, int]]):
    global F2L_MOVECOUNTS, F2L_ALGORITHMS, corners_to_indices, edges_to_indices
    if len(alg_so_far) == alg_length:
        if is_valid_case(cube3d):
            case = detect_case(cube3d)
            cornerind, edgeind = corners_to_indices[case[0]], edges_to_indices[case[1]]
            if F2L_MOVECOUNTS[cornerind][edgeind] == -1 or F2L_MOVECOUNTS[cornerind][edgeind] == alg_length:
                F2L_MOVECOUNTS[cornerind][edgeind] = alg_length
                ########## PLACE INVERTED ALGORITHM IN STASH ############
                mods = ("", "", "2", "'") # const lookup table (does rely on pythons index -1 dynamic)
                alg = [f'{"URFLBD"[a]}{mods[b]}' for a, b in alg_so_far]
                inverse = ['' for _ in range(len(alg))] # array of same length as alg
                for i, move in enumerate(alg[::-1]):
                    mv = move[0] # the first character, or the root word. Ex. if (move == "U'") then mv == "U"
                    if move[-1] == "2": inverse[i] = f'{mv}2'
                    elif move[-1] == "'": inverse[i] = f'{mv}'
                    else: inverse[i] = f"{mv}'"
                F2L_ALGORITHMS[cornerind][edgeind].append(' '.join(inverse))
                ##########################################################
        return
    opposites = (5, 3, 4, 1, 2, 0) # const lookup table
    move_types = [1, 1, 1, 1, 1, 1] # basically a bitfield
    # Do opposite-pairs explicitly
    #    UD
    #    RL
    #    FB
    # Don't choose the opposite of the last piece
    # Don't choose the same as the last piece
    move_types[alg_so_far[-1][0]] = 0
    move_types[opposites[alg_so_far[-1][0]]] = 0
    if move_types[0]:  # U
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,3:,:] = np.rot90(cube3d, k=b, axes=(2, 0))[:,3:,:] # execute U move
            DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(0, b)])
            if len(alg_so_far) <= alg_length-2: 
                for b2 in (1, -1, 2):
                    cube3dnewnew = np.copy(cube3dnew)
                    cube3dnewnew[:,:2,:] = np.rot90(cube3dnew, k=b2, axes=(0, 2))[:,:2,:] # execute opposite of U move or otherwise D
                    DFS_1tree_UDRLFB(cube3dnewnew, alg_length, alg_so_far + [(0, b), (5, b2)])
    if move_types[1]: # R
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[3:,:,:] = np.rot90(cube3d, k=b, axes=(1, 2))[3:,:,:] # execute R move
            DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(1, b)])
            if len(alg_so_far) <= alg_length-2:
                for b2 in (1, -1, 2):
                    cube3dnewnew = np.copy(cube3dnew)
                    cube3dnewnew[:2,:,:] = np.rot90(cube3dnew, k=b2, axes=(2, 1))[:2,:,:] # execute opposite of R move or otherwise L
                    DFS_1tree_UDRLFB(cube3dnewnew, alg_length, alg_so_far + [(1, b), (3, b2)])
    if move_types[2]: # F
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,:,:2] = np.rot90(cube3d, k=b, axes=(1, 0))[:,:,:2] # execute F move
            DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(2, b)])
            if len(alg_so_far) <= alg_length-2:
                for b2 in (1, -1, 2):
                    cube3dnewnew = np.copy(cube3dnew)
                    cube3dnewnew[:,:,3:] = np.rot90(cube3dnew, k=b2, axes=(0, 1))[:,:,3:] # execute opposite of F move or otherwise B
                    DFS_1tree_UDRLFB(cube3dnewnew, alg_length, alg_so_far + [(2, b), (4, b2)])
    if move_types[3]: # L
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:2,:,:] = np.rot90(cube3d, k=b, axes=(2, 1))[:2,:,:] # execute L move
            DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(3, b)])
    if move_types[4]: # B
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,:,3:] = np.rot90(cube3d, k=b, axes=(0, 1))[:,:,3:] # execute B move
            DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(4, b)])
    if move_types[5]: # D
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,:2,:] = np.rot90(cube3d, k=b, axes=(0, 2))[:,:2,:] # execute D move
            DFS_1tree_UDRLFB(cube3dnew, alg_length, alg_so_far + [(5, b)])


for k in range(3, 10):
    cube2ds = {'U': np.array([['U', 'U', 'U'], 
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
    cube3d = np.zeros(shape=(5, 5, 5), dtype=np.uint8)
    faces = "URFLBDN"
    for i in (-1, 0, 1):
        for j in (-1, 0, 1):
            cube3d[i+2, 4, j+2] = faces.find(cube2ds['U'][(1-j, i+1)])
            cube3d[4, j+2, i+2] = faces.find(cube2ds['R'][(1-j, i+1)])
            cube3d[i+2, j+2, 0] = faces.find(cube2ds['F'][(1-j, i+1)])
            cube3d[0, i+2, j+2] = faces.find(cube2ds['L'][(1-i, 1-j)])
            cube3d[i+2, j+2, 4] = faces.find(cube2ds['B'][(1-j, 1-i)])
            cube3d[i+2, 0, j+2] = faces.find(cube2ds['D'][(1-j, 1-i)])
    start = time.perf_counter()
    DFS_1tree_UDRLFB0(cube3d, k, [])
    t = time.perf_counter()-start
    for corner, cornerind in corners_to_indices.items():
        for edge, edgeind in edges_to_indices.items():
            if F2L_MOVECOUNTS[cornerind][edgeind] == k:
                algs: list[str] = F2L_ALGORITHMS[cornerind][edgeind]
                print(f'{corner}, {edge}: {algs[0]}', end='')
                if len(algs) > 1:
                    for e in algs[1:]:
                        print(f' | {e}', end='')
                print()
    print('######################################################################')
    print(f'{t} s')
    print('######################################################################')

'''
('B', 'U', 'L'), ('B', 'U'): F' U2 F
('U', 'L', 'B'), ('U', 'L'): R U2 R'
('R', 'U', 'B'), ('R', 'U'): F' U F
('F', 'U', 'R'), ('U', 'B'): R U R'
('U', 'R', 'F'), ('L', 'U'): F' U' F
('U', 'F', 'L'), ('U', 'F'): R U' R'
###################################################################### 6 cases can be done in 3 moves
0.016120199987199157 s
###################################################################### So far we have 6/383 cases completed
('B', 'U', 'L'), ('U', 'F'): U2 R U R'
('U', 'L', 'B'), ('R', 'U'): U2 F' U' F
('B', 'R', 'U'), ('B', 'R'): R2 F R2 F'
('R', 'U', 'B'), ('U', 'L'): U R U R'
('U', 'B', 'R'), ('U', 'B'): U2 R U' R' | U' R U2 R'
('U', 'B', 'R'), ('F', 'U'): U F' U' F
('F', 'U', 'R'), ('F', 'U'): F R' F' R | U' F' U F | U2 F' U2 F
('F', 'U', 'R'), ('B', 'R'): B' R2 B R2
('U', 'R', 'F'), ('U', 'R'): U R U' R' | U2 R U2 R' | R' F R F'
('U', 'R', 'F'), ('F', 'L'): L F2 L' F2
('F', 'L', 'U'), ('F', 'L'): F2 R' F2 R
('L', 'U', 'F'), ('U', 'R'): U' R U R'
('L', 'U', 'F'), ('L', 'U'): U2 F' U F | U F' U2 F
('U', 'F', 'L'), ('B', 'U'): U' F' U' F
###################################################################### 14 cases can be done in 4 moves
0.21753850003005937 s
###################################################################### So far we have 20/383 cases completed
('L', 'B', 'U'), ('U', 'R'): R B' R B R2
('L', 'B', 'U'), ('F', 'U'): F' L F' L' F2
('L', 'B', 'U'), ('L', 'B'): R B' U B R' | F' L U' L' F
('L', 'B', 'U'), ('B', 'R'): U R2 F R2 F'
('L', 'B', 'U'), ('F', 'L'): U' F2 R' F2 R
('B', 'U', 'L'), ('B', 'R'): U2 B' R2 B R2
('U', 'L', 'B'), ('F', 'L'): U2 L F2 L' F2
('B', 'R', 'U'), ('F', 'L'): U2 F2 R' F2 R
('R', 'U', 'B'), ('B', 'R'): U B' R2 B R2
('U', 'B', 'R'), ('F', 'L'): U L F2 L' F2
('R', 'F', 'U'), ('B', 'U'): F' L' U2 L F
('R', 'F', 'U'), ('U', 'L'): R B U2 B' R'
('R', 'F', 'U'), ('B', 'L'): R2 D B2 D' R2 | F2 D' L2 D F2
('R', 'F', 'U'), ('B', 'R'): R F R2 F' R' | R2 U R2 U' R2 | U' R2 F R2 F'
('R', 'F', 'U'), ('F', 'L'): U F2 R' F2 R | F' R' F2 R F | F2 U' F2 U F2
('F', 'U', 'R'), ('L', 'B'): B' R U R' B
('F', 'U', 'R'), ('R', 'B'): F D R D' F' | B R U R' B'
('U', 'R', 'F'), ('L', 'B'): L F' U' F L'
('U', 'R', 'F'), ('L', 'F'): R' D' F' D R | L' F' U' F L
('F', 'L', 'U'), ('B', 'R'): U2 R2 F R2 F'
('L', 'U', 'F'), ('B', 'R'): U' B' R2 B R2
('U', 'F', 'L'), ('F', 'L'): U' L F2 L' F2
('B', 'L', 'D'), ('B', 'L'): R2 B2 R' B2 R' | L R U2 L' R' | F2 L2 F L2 F | B' F' U2 B F | L F2 L F2 L2 | B' R2 B' R2 B2
('B', 'L', 'D'), ('B', 'R'): B' U2 R2 B R2
('B', 'L', 'D'), ('F', 'L'): L U2 F2 L' F2
('L', 'D', 'B'), ('U', 'L'): R B' U2 B R' | B' R U2 R' B
('D', 'B', 'L'), ('B', 'U'): F' L U2 L' F | L F' U2 F L'
('B', 'D', 'R'), ('U', 'B'): R' U2 R2 U' R' | R' U' R2 U2 R'
('D', 'R', 'B'), ('R', 'U'): B F' U B' F
('F', 'R', 'D'), ('B', 'L'): L2 D F2 D' L2 | B2 D' R2 D B2
('F', 'R', 'D'), ('B', 'R'): R' B' R2 B R | R2 U' R2 U R2 | R2 U2 R2 U2 R2 | L2 D2 R2 D2 L2
('F', 'R', 'D'), ('R', 'B'): F' R' U R F | B' D' R D B
('F', 'R', 'D'), ('F', 'L'): F L F2 L' F' | F2 U F2 U' F2 | F2 U2 F2 U2 F2 | B2 D2 F2 D2 B2
('F', 'R', 'D'), ('L', 'F'): R F U' F' R' | L D F' D' L'
('F', 'D', 'L'), ('U', 'F'): L' R U' L R'
('D', 'L', 'F'), ('L', 'U'): F U2 F2 U F | F U F2 U2 F
###################################################################### 36 cases can be done in 5 moves
2.9257274999981746 s
###################################################################### So far we have 56/383 cases completed
('L', 'B', 'U'), ('B', 'L'): U2 R2 D B2 D' R2 | U2 F2 D' L2 D F2
('B', 'U', 'L'), ('R', 'U'): F' L' U' L U2 F
('B', 'U', 'L'), ('L', 'B'): U2 B' R U R' B
('B', 'U', 'L'), ('R', 'B'): U2 F D R D' F' | F' U2 R' U R F | U2 B R U R' B'
('B', 'U', 'L'), ('F', 'R'): F' L' U2 L U2 F
('B', 'U', 'L'), ('L', 'F'): L' U L F' U2 F
('U', 'L', 'B'), ('U', 'F'): R B U B' U2 R'
('U', 'L', 'B'), ('L', 'B'): U2 L F' U' F L'
('U', 'L', 'B'), ('R', 'B'): B U' B' R U2 R'
('U', 'L', 'B'), ('F', 'R'): R B U2 B' U2 R'
('U', 'L', 'B'), ('L', 'F'): R U2 F U' F' R' | U2 R' D' F' D R | U2 L' F' U' F L
('B', 'R', 'U'), ('U', 'F'): U R B U2 B' R' | U' R B' R B R2
('B', 'R', 'U'), ('F', 'U'): F' U2 F2 R' F' R | F' U' L F' L' F2
('B', 'R', 'U'), ('L', 'U'): F' L' U' L U' F | U F' L' U2 L F | U' F' L F' L' F2
('B', 'R', 'U'), ('B', 'L'): U R2 D B2 D' R2 | U F2 D' L2 D F2
('B', 'R', 'U'), ('L', 'B'): U' R B' U B R' | F' U' L U' L' F | U' F' L U' L' F
('B', 'R', 'U'), ('R', 'F'): F' U2 F R U R'
('R', 'U', 'B'), ('B', 'L'): L U L' R U R' | L U R U L' R' | B' U B F' U F | B' F' U B U F
('R', 'U', 'B'), ('L', 'B'): U B' R U R' B
('R', 'U', 'B'), ('R', 'B'): U F D R D' F' | F' U R' U R F | U B R U R' B'
('R', 'U', 'B'), ('F', 'L'): L' U' L R U' R' | L' U L R U R' | L' U R U L R'
('U', 'B', 'R'), ('L', 'B'): U L F' U' F L'
('U', 'B', 'R'), ('F', 'R'): F' U2 L' U2 L F
('U', 'B', 'R'), ('L', 'F'): U R' D' F' D R | U L' F' U' F L
('R', 'F', 'U'), ('L', 'B'): U2 R B' U B R' | U2 F' L U' L' F
('R', 'F', 'U'), ('R', 'F'): F' U F R U2 R' | R U' R' F' U2 F
('F', 'U', 'R'), ('U', 'L'): F2 U' L' U L F2
('F', 'U', 'R'), ('L', 'U'): R2 B U B' U' R2
('F', 'U', 'R'), ('L', 'F'): F U2 F' R U R' | R U F U' F' R'
('U', 'R', 'F'), ('U', 'B'): F2 L' U' L U F2
('U', 'R', 'F'), ('B', 'U'): R2 U B U' B' R2
('U', 'R', 'F'), ('R', 'B'): R' U2 R F' U' F | F' U' R' U R F
('F', 'L', 'U'), ('U', 'B'): R B U B' U R' | U' R B U2 B' R' | U R B' R B R2
('F', 'L', 'U'), ('U', 'R'): R U B' R B R2 | R U2 R2 F R F'
('F', 'L', 'U'), ('R', 'U'): U' F' L' U2 L F | U F' L F' L' F2
('F', 'L', 'U'), ('B', 'L'): U' R2 D B2 D' R2 | U' F2 D' L2 D F2
('F', 'L', 'U'), ('L', 'B'): R U B' U B R' | U R B' U B R' | U F' L U' L' F
('F', 'L', 'U'), ('R', 'F'): R U2 R' F' U' F
('L', 'U', 'F'), ('L', 'B'): U' B' R U R' B
('L', 'U', 'F'), ('R', 'B'): U' F D R D' F' | U' B R U R' B'
('L', 'U', 'F'), ('F', 'R'): R U2 B U2 B' R'
('U', 'F', 'L'), ('B', 'L'): L U' L' R U' R' | L R U' L' U' R' | B' U' B F' U' F | B' U' F' U' B F
('U', 'F', 'L'), ('L', 'B'): U' L F' U' F L'
('U', 'F', 'L'), ('B', 'R'): B U' B' F' U' F | B U B' F' U F | B U' F' U' B' F
('U', 'F', 'L'), ('L', 'F'): R U' F U' F' R' | U' R' D' F' D R | U' L' F' U' F L
('B', 'L', 'D'), ('U', 'B'): B' U2 B R U R'
('B', 'L', 'D'), ('R', 'U'): L U' L' F' U2 F | F' L U' L' U2 F | L U2 F' U' F L'
('B', 'L', 'D'), ('U', 'F'): B' U B R U2 R' | R B' U B U2 R' | B' U2 R U R' B
('B', 'L', 'D'), ('L', 'U'): L U2 L' F' U' F
('L', 'D', 'B'), ('U', 'B'): R U' B' U2 B R' | U' R B' U2 B R' | U' B' R U2 R' B
('L', 'D', 'B'), ('U', 'R'): U2 R B' U2 B R' | L2 U' R U L2 R' | R B' U R B R2 | U2 B' R U2 R' B
('L', 'D', 'B'), ('R', 'U'): B' U2 B F' U' F | B' U2 F' U' B F | B2 R B U R' B | B2 F' U F U' B2
('L', 'D', 'B'), ('U', 'F'): R U B' U2 B R' | U R B' U2 B R' | U B' R U2 R' B
('L', 'D', 'B'), ('B', 'R'): B' R U R B R2 | B2 R B R B R2
('L', 'D', 'B'), ('F', 'R'): R U2 B' U2 B R' | R2 U' L U L' R2 | B' F2 U B U' F2
('D', 'B', 'L'), ('R', 'U'): F' U' L U2 L' F | U' F' L U2 L' F | U' L F' U2 F L'
('D', 'B', 'L'), ('U', 'F'): L U2 L' R U R' | L U2 R U L' R' | L2 F' L' U' F L' | L2 R U' R' U L2
('D', 'B', 'L'), ('F', 'U'): U2 F' L U2 L' F | B2 U F' U' B2 F | F' L U' F' L' F2 | U2 L F' U2 F L'
('D', 'B', 'L'), ('L', 'U'): F' U L U2 L' F | U F' L U2 L' F | U L F' U2 F L'
('D', 'B', 'L'), ('F', 'R'): L R2 U' L' U R2 | F' U2 L U2 L' F | F2 U B' U' B F2
('D', 'B', 'L'), ('F', 'L'): L F' U' F' L' F2 | L2 F' L' F' L' F2
('R', 'B', 'D'), ('F', 'U'): B U B' F' U' F | B U' B' F' U F | B U F' U' B' F
('R', 'B', 'D'), ('R', 'B'): F R2 F' R' U' R' | B U2 B' R U' R' | B' R B2 U B' R' | R' F' U F2 R F' | F' R' U2 R U' F | F' R' U' R U2 F | R' U' R F' U2 F | B U2 R U' R' B' | B U' R U2 R' B' | R' U' R' B' R2 B
('R', 'B', 'D'), ('F', 'R'): B F' U' B' U F
('B', 'D', 'R'), ('U', 'R'): U' R' U2 R2 U' R' | U' R' U' R2 U2 R'
('B', 'D', 'R'), ('U', 'F'): U2 R' U2 R2 U' R' | U2 R' U' R2 U2 R'
('B', 'D', 'R'), ('U', 'L'): U R' U2 R2 U' R' | U R' U' R2 U2 R'
('B', 'D', 'R'), ('R', 'F'): B' R2 B R' U R'
('B', 'D', 'R'), ('F', 'L'): F' R' U2 F2 R F
('D', 'R', 'B'), ('U', 'B'): B U B' R U R'
('D', 'R', 'B'), ('B', 'U'): U B F' U B' F | F' U B U B' F
('D', 'R', 'B'), ('F', 'U'): U' B F' U B' F
('D', 'R', 'B'), ('U', 'L'): B U R U R' B'
('D', 'R', 'B'), ('L', 'U'): B U B2 R2 B R2 | U2 B F' U B' F | F' U2 B U B' F
('D', 'R', 'B'), ('B', 'L'): B R' B R B' R | B U R2 B R2 B'
('D', 'R', 'B'), ('F', 'R'): F' U' B U B' F
('D', 'R', 'B'), ('F', 'L'): L R2 F2 L' R2 F2
('F', 'R', 'D'), ('L', 'B'): R2 B' R' U B R' | F2 L F U' L' F
('R', 'D', 'F'), ('U', 'B'): R B U2 B' U R'
('R', 'D', 'F'), ('B', 'U'): R U' R' F' U' F | F2 L F L' U2 F
('R', 'D', 'F'), ('U', 'R'): F' U2 F R U2 R' | R U2 B' R B R2 | R U' R2 F R F'
('R', 'D', 'F'), ('L', 'U'): R' F R F2 U' F | F' U L' U2 L F
('R', 'D', 'F'), ('L', 'B'): R U2 B' U B R' | F' L U L' U2 F
('R', 'D', 'F'), ('F', 'L'): R U L' U2 L R' | F' U R' F2 R F
('D', 'F', 'R'), ('U', 'B'): F R' F' R2 U R' | R U' B U2 B' R'
('D', 'F', 'R'), ('F', 'U'): F' U F2 R' F' R | R U2 R' F' U2 F | F' U2 L F' L' F2
('D', 'F', 'R'), ('U', 'L'): F' U F R U R' | R2 B' R' B U2 R'
('D', 'F', 'R'), ('L', 'U'): F' L' U2 L U' F
('D', 'F', 'R'), ('L', 'B'): R B' U' B U2 R' | F' U2 L U' L' F
('D', 'F', 'R'), ('B', 'R'): R U' F R2 F' R' | F' U' B U2 B' F
('L', 'F', 'D'), ('U', 'R'): L' U L R U' R' | L' U' L R U R' | L' U' R U L R'
('L', 'F', 'D'), ('F', 'R'): L' R U L U' R'
('L', 'F', 'D'), ('L', 'F'): R F U2 F' U R' | F U F' R U2 R' | R F U F' U2 R' | F R U' R2 F' R | L' U2 L F' U F | R' F2 R F U F | L F' L2 U' L F | F U F L F2 L' | L' U2 F' U F L | L' U F' U2 F L
('F', 'D', 'L'), ('U', 'B'): U2 L' R U' L R' | R U2 L' U' L R' | L' U' L2 F2 L' F2
('F', 'D', 'L'), ('B', 'U'): L' U' F' U' F L
('F', 'D', 'L'), ('U', 'R'): U L' R U' L R'
('F', 'D', 'L'), ('U', 'L'): U' L' R U' L R' | R U' L' U' L R'
('F', 'D', 'L'), ('L', 'U'): L' U' L F' U' F
('F', 'D', 'L'), ('B', 'L'): L' F L' F' L F' | L' U' F2 L' F2 L
('F', 'D', 'L'), ('B', 'R'): B' F2 R2 B F2 R2
('F', 'D', 'L'), ('F', 'R'): R U L' U' L R'
('D', 'L', 'F'), ('B', 'U'): U' F U2 F2 U F | U' F U F2 U2 F
('D', 'L', 'F'), ('R', 'U'): U2 F U2 F2 U F | U2 F U F2 U2 F
('D', 'L', 'F'), ('F', 'U'): U F U2 F2 U F | U F U F2 U2 F
('D', 'L', 'F'), ('B', 'R'): R F U2 R2 F' R'
('D', 'L', 'F'), ('R', 'F'): L F2 L' F U' F
###################################################################### 106 cases can be done in 6 moves
39.67212169995764 s
###################################################################### So far we have 162/383 cases completed
('L', 'B', 'U'), ('U', 'B'): R2 U2 R' U' R U' R2 | F' L' U' L2 F' L' F2
('L', 'B', 'U'), ('B', 'U'): F2 U2 R' F' R U2 F2 | F2 L2 B L B' L F2
('L', 'B', 'U'), ('R', 'U'): U F' U2 F2 R' F' R | F' U2 L' U2 L U' F | U F' U' L F' L' F2
('L', 'B', 'U'), ('U', 'F'): R U2 B U2 B' U R' | U' R U B' R B R2 | U' R U2 R2 F R F'
('L', 'B', 'U'), ('U', 'L'): R2 U2 F R F' U2 R2 | R2 B2 L' B' L B' R2
('L', 'B', 'U'), ('L', 'U'): R B U B2 R B R2 | F2 U2 F U F' U F2
('L', 'B', 'U'), ('R', 'B'): R B' R' B R U R' | R2 U R B U' B' R2 | F' U' F2 D R D' F'
('L', 'B', 'U'), ('F', 'R'): R U' B U B' U R' | R2 U2 R2 U' R2 U' R2 | L' U R2 U' L U R2 | F' U L' U' L U' F | B U' F2 U B' U' F2 | F2 U2 F2 U F2 U F2
('L', 'B', 'U'), ('R', 'F'): U F' U2 F R U R' | U2 F' U F R U2 R' | R B U' B2 R B R2 | U' R U2 R' F' U' F | U2 R U' R' F' U2 F | F' L' U L2 F' L' F2
('L', 'B', 'U'), ('L', 'F'): R U R2 D' F' D R | F' L F L' F' U' F | F2 U' F' L' U L F2
('B', 'U', 'L'), ('U', 'R'): R U' R' U' R U R' | U2 F2 U' L' U L F2 | L F' R' F' R F L'
('B', 'U', 'L'), ('U', 'L'): R2 U R' U R U2 R2
('B', 'U', 'L'), ('B', 'L'): U L U L' R U R' | U L U R U L' R' | R U R D B2 D' R2 | U B' U B F' U F | U B' F' U B U F | F' U B' U B U F | D2 F2 L2 F L2 F D2 | D2 B' R2 B' R2 B2 D2
('B', 'U', 'L'), ('R', 'F'): F' U' F U R U' R' | F' U F U R U R' | F' U' F U2 R U2 R' | F' U' F R' F R F'
('B', 'U', 'L'), ('F', 'L'): U L' U' L R U' R' | R U L' U' L U' R' | L' U' L U2 R U R' | U L' U L R U R' | U L' U R U L R' | F' U2 F2 L F2 L' F' | F2 U' F' U F U' F | F' U' F2 U F2 U' F | L' U' L2 F' L' U' F | F' L R' F2 L' R F | F' U2 F' U F2 U' F2 | F' U2 F' U2 F2 U2 F2 | L F R' F L' R F2 | F' U' F L F2 L' F2 | L F L' F' L F' L' | F' L R' F2 R F L'
('U', 'L', 'B'), ('B', 'U'): F2 U' F U' F' U2 F2
('U', 'L', 'B'), ('F', 'U'): U2 R2 U B U' B' R2 | F' U F U F' U' F | B' R F R F' R' B
('U', 'L', 'B'), ('B', 'L'): U' L U' L' R U' R' | U' L R U' L' U' R' | R U' L U' L' U' R' | U' B' U' B F' U' F | U' B' U' F' U' B F | F' U' F' D' L2 D F2 | D2 R2 B2 R' B2 R' D2 | D2 L F2 L F2 L2 D2
('U', 'L', 'B'), ('B', 'R'): R2 U R U' R' U R' | R U R2 U' R2 U R' | B U B2 R B U R' | R B' F R2 B F' R' | R U2 R2 B' R2 B R | R U2 R U' R2 U R2 | R U2 R U2 R2 U2 R2 | B' R' F R' B F' R2 | R U R' B' R2 B R2 | B U B' U2 F' U' F | U' B U' B' F' U' F | U' B U B' F' U F | F' U' B U B' U F | U' B U' F' U' B' F | R B' F R2 F' R' B | B' R' B R B' R B
('U', 'L', 'B'), ('R', 'F'): R U R' F R' F' R | R U' R' U' F' U' F | R U R' U' F' U F | R U R' U2 F' U2 F
('B', 'R', 'U'), ('U', 'L'): R2 B' R' B2 U2 B' R' | U2 R U B' R B R2 | U2 R U2 R2 F R F'
('B', 'R', 'U'), ('R', 'B'): F' U2 F2 D R D' F'
('B', 'R', 'U'), ('F', 'R'): F' U L' U2 L U' F
('B', 'R', 'U'), ('L', 'F'): L' U L2 F' L' U2 F
('R', 'U', 'B'), ('U', 'B'): R U' R' U R U R' | R2 D R' U R D' R2
('R', 'U', 'B'), ('U', 'F'): R U R' U R U R' | F' U' F U2 R U R' | F' U' L' U L U2 F | U F2 U' L' U L F2
('R', 'U', 'B'), ('F', 'U'): U R2 B U B' U' R2 | U' F' L' U' L U2 F | F2 U2 R' F R U2 F2 | L F' R' F R F L'
('R', 'U', 'B'), ('L', 'U'): F' U' L' U2 L U2 F
('R', 'U', 'B'), ('F', 'R'): R U2 R' U R U R' | U2 R U2 B U2 B' R' | R' D' R U R' D R | B' U F' U' B U F | F' U' L' U' L U2 F | U' F' L' U2 L U2 F | L D' L' U2 L D L' | B D2 B' U' B D2 B'
('R', 'U', 'B'), ('L', 'F'): U F U2 F' R U R' | U R U F U' F' R' | F' U R' F' R U F | U' L' U L F' U2 F
('U', 'B', 'R'), ('B', 'U'): R U2 R' U F' U' F
('U', 'B', 'R'), ('U', 'F'): R U2 R' U2 R U' R' | R U2 B U' B' U R' | R U2 R' U' R U2 R'
('U', 'B', 'R'), ('U', 'L'): R U R' U2 R U' R' | R U B U' B' U R' | R U R' U' R U2 R' | U' R B U B' U2 R' | U F2 L' U' L U F2
('U', 'B', 'R'), ('L', 'U'): U R2 U B U' B' R2 | R U' R' U F' U' F
('U', 'B', 'R'), ('B', 'L'): U2 L U' L' R U' R' | U2 L R U' L' U' R' | R2 B R' B2 R B' R2 | B' U2 B U F' U' F | U2 B' U' B F' U' F | U2 B' U' F' U' B F | F' U2 F' D' L2 D F2
('U', 'B', 'R'), ('B', 'R'): R' F' U' F R2 U R' | U2 B U' B' F' U' F | U2 B U B' F' U F | F' U2 B U B' U F | F' R2 D' F' D R2 F | U2 B U' F' U' B' F
('U', 'B', 'R'), ('R', 'B'): U' B U' B' R U2 R' | U R' U2 R F' U' F | U F' U' R' U R F
('U', 'B', 'R'), ('R', 'F'): R B' R B R' U R' | R U R' U F' U' F
('R', 'F', 'U'), ('U', 'B'): U R U B' R B R2 | U R U2 R2 F R F'
('R', 'F', 'U'), ('U', 'R'): R U2 R' U' R U R'
('R', 'F', 'U'), ('F', 'U'): F' U2 F U F' U' F
('R', 'F', 'U'), ('L', 'U'): U' F' U2 F2 R' F' R | U' F' U' L F' L' F2
('R', 'F', 'U'), ('R', 'B'): B' R B2 U' B' U2 R'
('R', 'F', 'U'), ('F', 'R'): R2 U R2 U R2 U2 R2 | F2 U' F2 U' F2 U2 F2
('R', 'F', 'U'), ('L', 'F'): L F' L2 U L U2 F
('F', 'U', 'R'), ('B', 'U'): R2 U2 F R2 F' U2 R2 | F' U' L' U2 L U' F | L' B2 R2 B R2 B L | B' R' U' R2 U R' B
('F', 'U', 'R'), ('R', 'U'): R U' B U' B' U2 R' | R2 B' D B' D' B2 R2 | R U' R' U2 F' U' F | F' U2 L' U' L U' F | F' L' B' U B L F
('F', 'U', 'R'), ('U', 'F'): R U' B U2 B' U2 R' | R' U2 R2 U R2 U R
('F', 'U', 'R'), ('B', 'L'): R U2 L U' L' U' R' | U' L U L' R U R' | U' L U R U L' R' | F L2 U2 F' U2 L2 F' | U' B' U B F' U F | U' B' F' U B U F | F2 L' D' L2 D L' F2 | F2 L U' L2 U L F2 | B D2 L2 F' L2 D2 B' | B2 R' U' R2 U R' B2 | B2 R D' R2 D R B2
('F', 'U', 'R'), ('F', 'R'): U R U2 B U2 B' R' | U2 F' L' U2 L U2 F | L D' L' U L D L' | B D2 B' U2 B D2 B'
('F', 'U', 'R'), ('F', 'L'): U' L' U' L R U' R' | U' L' U L R U R' | U' L' U R U L R' | R2 B2 L2 B' L2 B' R2
('U', 'R', 'F'), ('R', 'U'): F U2 F2 U' F2 U' F' | F' U L' U2 L U2 F
('U', 'R', 'F'), ('U', 'F'): F' U F U2 R U R' | R U2 B U B' U R' | R B L U' L' B' R' | F' U L' U L U2 F | F2 L D' L D L2 F2
('U', 'R', 'F'), ('U', 'L'): R U B U2 B' U R' | F2 U2 R' F2 R U2 F2 | L F U F2 U' F L' | B L2 F2 L' F2 L' B'
('U', 'R', 'F'), ('B', 'L'): U L U' L' R U' R' | U L R U' L' U' R' | R' B2 U2 R U2 B2 R | R2 B' U B2 U' B' R2 | R2 B D B2 D' B R2 | U B' U' B F' U' F | F' U2 B' U B U F | U B' U' F' U' B F | L' D2 B2 R B2 D2 L | L2 F' D F2 D' F' L2 | L2 F U F2 U' F L2
('U', 'R', 'F'), ('B', 'R'): U B U' B' F' U' F | U B U B' F' U F | U B U' F' U' B' F | F2 L2 B2 L B2 L F2
('U', 'R', 'F'), ('F', 'R'): U2 R B U2 B' U2 R' | U' F' U2 L' U2 L F | L' D2 L U2 L' D2 L | B' D B U' B' D' B
('F', 'L', 'U'), ('B', 'U'): U2 F' U2 F2 R' F' R | F2 L F L2 U2 L F | U2 F' U' L F' L' F2
('F', 'L', 'U'), ('R', 'B'): B U' B2 R B U2 R'
('F', 'L', 'U'), ('F', 'R'): R U' B U2 B' U R'
('F', 'L', 'U'), ('L', 'F'): R U2 R2 D' F' D R
('L', 'U', 'F'), ('U', 'B'): F' U F U' R U R' | U' F2 U' L' U L F2
('L', 'U', 'F'), ('B', 'U'): U' R2 B U B' U' R2 | F' U' L' U L U' F | F' U' F U2 F' U F | F' U' F U F' U2 F | U F' L' U' L U2 F
('L', 'U', 'F'), ('R', 'U'): F' U2 L' U L U' F | F' U2 F U2 F' U F | F' U2 F U F' U2 F
('L', 'U', 'F'), ('U', 'L'): F' U2 F U' R U R'
('L', 'U', 'F'), ('B', 'L'): L U2 L' U' R U R' | U2 L U L' R U R' | U2 L U R U L' R' | R U2 R D B2 D' R2 | U2 B' U B F' U F | U2 B' F' U B U F | F2 L' F L2 F' L F2
('L', 'U', 'F'), ('R', 'F'): F' U' F U' R U R' | F' L F' L' F U' F
('L', 'U', 'F'), ('F', 'L'): U2 L' U' L R U' R' | R U2 L' U' L U' R' | U2 L' U L R U R' | R F2 D R D' F2 R' | U2 L' U R U L R' | F R U R' F2 U' F
('L', 'U', 'F'), ('L', 'F'): U' F U2 F' R U R' | U' R U F U' F' R' | U L' U L F' U2 F
('U', 'F', 'L'), ('U', 'B'): R U B U2 B' U2 R'
('U', 'F', 'L'), ('U', 'R'): U R B U B' U2 R' | R2 U2 F R' F' U2 R2 | U' F2 L' U' L U F2 | B' R F R' F' R' B
('U', 'F', 'L'), ('R', 'U'): R U B U' B' U2 R' | U' R2 U B U' B' R2 | F' U' F U' F' U' F | R U R' U2 F' U' F
('U', 'F', 'L'), ('L', 'U'): F' U F U' F' U' F | F2 D' F U' F' D F2
('U', 'F', 'L'), ('R', 'B'): R U' F R F' U' R' | U B U' B' R U2 R' | U' R' U2 R F' U' F | U' F' U' R' U R F
('U', 'F', 'L'), ('F', 'R'): L U' R U L' U' R' | R U B U B' U2 R' | U R B U2 B' U2 R' | F D F' U' F D' F' | F' U2 F U' F' U' F | U2 F' U2 L' U2 L F | L' D2 L U L' D2 L | B' D B U2 B' D' B
('B', 'L', 'D'), ('B', 'U'): U' L U2 L' F' U' F | U L U' L' F' U2 F | F' L2 F' L2 F U2 F | F' U L U' L' U2 F | U F' L U' L' U2 F | B' U2 F' U2 B U2 F | U L U2 F' U' F L' | L2 F' L2 U2 L F L' | L2 F' L2 U2 L2 F L2 | B' U2 B' R2 B R2 B | D2 B2 R2 B R2 B D2
('B', 'L', 'D'), ('U', 'R'): U' B' U2 B R U R' | U B' U B R U2 R' | U R B' U B U2 R' | R B' R' U2 R2 B R2 | F2 L' F2 U2 F2 L F2 | U B' U2 R U R' B | B' R2 U R' U' R2 B | D2 R B2 R B2 R2 D2
('B', 'L', 'D'), ('F', 'U'): R2 B R2 U2 R2 B' R2 | U L U2 L' F' U' F | U' L U' L' F' U2 F | U' F' L U' L' U2 F | F' L F U2 F2 L' F2 | U' L U2 F' U' F L' | L F2 U' F U F2 L' | D2 F' L2 F' L2 F2 D2
('B', 'L', 'D'), ('U', 'L'): U B' U2 B R U R' | R B2 R B2 R' U2 R' | U' B' U B R U2 R' | L U2 R U2 L' U2 R' | R U' B' U B U2 R' | U' R B' U B U2 R' | L U2 L F2 L' F2 L' | U' B' U2 R U R' B | B2 R B2 U2 B' R' B | B2 R B2 U2 B2 R' B2 | D2 L2 F2 L' F2 L' D2
('B', 'L', 'D'), ('R', 'B'): B2 R B' U' B' U2 R' | B U' B R B2 U2 R' | B2 U' R B' U B2 R' | B' U2 B2 R U R' B' | B2 U' R B' U R' B2
('B', 'L', 'D'), ('F', 'R'): R U B' U B U2 R' | R2 B' R2 U2 R2 B R2 | F' U' L U' L' U2 F | F2 L F2 U2 F2 L' F2 | L D' F U' F' D L' | L2 F L2 U2 L2 F' L2 | B' D R' U R D' B | B2 R' B2 U2 B2 R B2
('B', 'L', 'D'), ('L', 'F'): L2 F' L U L U2 F | L' U L' F' L2 U2 F | L2 U F' L U' L2 F | L U2 L2 F' U' F L | L2 U F' L U' F L2
('L', 'D', 'B'), ('B', 'U'): L' B L B' R U2 R' | L' R B L B' U2 R' | B' U' B U' F' U' F | U B' U2 B F' U' F | U B' U2 F' U' B F | U B2 R B U R' B | U B2 F' U F U' B2 | F' U B2 U F U' B2 | B2 R' B R B' R B2 | D2 F' L U2 L' F D2 | D2 L F' U2 F L' D2
('L', 'D', 'B'), ('F', 'U'): L U' L' U2 F' U' F | U' B' U2 B F' U' F | U' B' U2 F' U' B F | L F' L F' L' F2 L' | U' B2 R B U R' B | U' B2 F' U F U' B2 | B2 R F R F' R' B2
('L', 'D', 'B'), ('L', 'U'): U2 B' U2 B F' U' F | B F' L2 B' L2 U' F | U2 B' U2 F' U' B F | U2 B2 R B U R' B | U2 B2 F' U F U' B2 | F' U2 B2 U F U' B2
('L', 'D', 'B'), ('R', 'B'): B U' B2 R U2 B R' | B U' B2 R U2 R' B
('L', 'D', 'B'), ('F', 'L'): B' F2 U B F2 U' F2 | L U R' F2 L' R F2 | L R U R' F2 L' F2 | B' U2 B L F2 L' F2 | B' U2 L F2 L' B F2 | F2 L' U' F2 U F2 L | F2 U' L' F2 U F2 L | F' L2 D F2 D' F' L2 | B' F2 U B D' L2 D
('D', 'B', 'L'), ('U', 'B'): U2 L U2 L' R U R' | L' R B2 L B2 U R' | U2 L U2 R U L' R' | U2 L2 F' L' U' F L' | U2 L2 R U' R' U L2 | R U2 L2 U' R' U L2
('D', 'B', 'L'), ('U', 'R'): B' U B U2 R U R' | U L U2 L' R U R' | U L U2 R U L' R' | U L2 F' L' U' F L' | U L2 R U' R' U L2 | L2 F' R' F' R F L2 | B' R B' R B R2 B
('D', 'B', 'L'), ('U', 'L'): L U L' U R U R' | U' L U2 L' R U R' | U' L U2 R U L' R' | B L' B' L F' U2 F | B F' L' B' L U2 F | U' L2 F' L' U' F L' | U' L2 R U' R' U L2 | R U' L2 U' R' U L2 | L2 F L' F' L F' L2 | D2 R B' U2 B R' D2 | D2 B' R U2 R' B D2
('D', 'B', 'L'), ('B', 'R'): L R2 U' L' R2 U R2 | B' U' F R2 B F' R2 | B' F' U' F R2 B R2 | L U2 L' B' R2 B R2 | L U2 B' R2 B L' R2 | R2 B U R2 U' R2 B' | R2 U B R2 U' R2 B' | R B2 D' R2 D R B2 | L R2 U' L' D B2 D'
('D', 'B', 'L'), ('L', 'F'): L' U L2 F' U2 L' F | L' U L2 F' U2 F L'
('R', 'B', 'D'), ('B', 'U'): B' R2 B U R2 U' R2 | B U2 B' U' F' U' F | U2 B U B' F' U' F | U2 B U' B' F' U F | F' U2 B U' B' U F | U2 B U F' U' B' F
('R', 'B', 'D'), ('U', 'R'): R' U2 R U' R U R' | R' U' R' U R' U' R2 | B' R' B R' B' R B
('R', 'B', 'D'), ('R', 'U'): U B U B' F' U' F | U B U' B' F' U F | F' U B U' B' U F | U B U F' U' B' F
('R', 'B', 'D'), ('U', 'F'): F' D2 L' U' L D2 F
('R', 'B', 'D'), ('U', 'L'): R' U R U2 R U' R' | R' U' R U2 R U R' | R' U R U' R U2 R'
('R', 'B', 'D'), ('L', 'U'): F R F2 U' F2 R' F' | U' B U B' F' U' F | U' B U' B' F' U F | F' U' B U' B' U F | U' B U F' U' B' F
('R', 'B', 'D'), ('B', 'L'): B2 R2 B2 U R2 U' R2 | B2 U B2 U' R2 B2 R2 | B' R2 B D B2 D' R2 | F' D B' U2 B D' F | B R B2 R B' R2 B' | B R B' R B2 R2 B' | F2 D L2 D' F2 U B2
('R', 'B', 'D'), ('F', 'L'): L' R' U' L R2 U' R' | R2 D' F2 D F R2 F' | B F U' B' F2 U F | R2 F2 U' F2 U R2 F2 | R2 D' F2 D F2 R2 F2 | B U B' L F2 L' F2 | B L U F2 L' B' F2 | B U L F2 L' B' F2 | F2 U B2 D' L2 D B2
('B', 'D', 'R'), ('B', 'U'): F2 D2 L2 B L2 D2 F2 | B2 U2 R2 B R2 U2 B2
('B', 'D', 'R'), ('R', 'U'): B' R B2 U' B' U R' | B' R D' R D B R2 | B' R2 D B' D' B2 R2 | R' U' R U2 F' U' F
('B', 'D', 'R'), ('F', 'U'): F R2 F2 U2 F U2 R2 | B' R2 B2 U2 B' U2 R2 | R' U F' U' F2 R F' | R2 U' F' U F2 R2 F' | R2 U2 F' U2 F2 R2 F' | R2 F2 D' F' D R2 F' | R' U2 R U' F' U' F | F2 U2 R2 F R2 U2 F2 | R2 U2 B U2 B2 R2 B | B2 D2 L2 F L2 D2 B2
('B', 'D', 'R'), ('L', 'U'): B' R2 B2 U B' U' R2 | B U' B' U F' U' F | B U F' U B' U F
('B', 'D', 'R'), ('B', 'L'): R2 F D2 B2 D2 F' R2 | R2 B R2 B2 R2 B' R2 | R2 F D2 B D2 R2 F'
('B', 'D', 'R'), ('F', 'R'): R U' R2 U2 R2 U' R' | R U' R2 U' R2 U2 R' | F2 U R' U R U2 F2
('D', 'R', 'B'), ('U', 'R'): U' B U B' R U R' | R' U' R2 B' R B R2 | B' R2 B R F R F' | U2 B U R U R' B' | B' R2 D' R D R B
('D', 'R', 'B'), ('U', 'F'): R' U R U R U R' | U2 B U B' R U R' | U B U R U R' B'
('D', 'R', 'B'), ('L', 'B'): B U B2 R U B R' | R' U' R2 B' U B R' | B U B2 R U R' B | R' U' R2 B' U R' B
('D', 'R', 'B'), ('R', 'B'): R' B' R B R2 U R' | R' U R2 F R F' R'
('D', 'R', 'B'), ('L', 'F'): R' D' F2 U F D R | F' R' F' U R U F
('F', 'R', 'D'), ('U', 'B'): F' U F U R U' R' | F' U' F U R U R' | R' F R F' R U R' | F' U F U2 R U2 R' | R U2 B U B' U2 R' | F' U F R' F R F' | L' B2 D' R D B2 L
('F', 'R', 'D'), ('B', 'U'): F U2 L F2 L' U2 F' | F' U2 L' U2 L U2 F | B' D' R2 D B2 U' B' | B L2 D F2 D' L2 B' | B' R2 U' R2 U R2 B | B U B2 D' R2 D B
('F', 'R', 'D'), ('U', 'R'): R' U2 B' R' B U2 R | F U2 L F' L' U2 F' | L' B2 D' R' D B2 L | B' R2 U' R' U R2 B
('F', 'R', 'D'), ('R', 'U'): R U2 B U' B' U2 R' | R U2 R' U2 F' U' F | L F2 U F' U' F2 L'
('F', 'R', 'D'), ('U', 'F'): F' U2 F U2 R U R' | F' U2 L' U L U2 F | B' R2 U' R U R2 B
('F', 'R', 'D'), ('F', 'U'): R' U2 B' R B U2 R | F U2 L F L' U2 F' | L F2 U F U' F2 L' | B L2 D F D' L2 B'
('F', 'R', 'D'), ('U', 'L'): R U2 B U2 B' U2 R' | R' U2 B' R2 B U2 R | L F2 U F2 U' F2 L' | L' U' L2 D F2 D' L' | L D F2 D' L2 U L | L' B2 D' R2 D B2 L
('F', 'R', 'D'), ('L', 'U'): R U' R' F R' F' R | R U R' U' F' U' F | F R' F' R F' U' F | R U' R' U' F' U F | R U' R' U2 F' U2 F | F' U2 L' U' L U2 F | B L2 D F' D' L2 B'
('R', 'D', 'F'), ('R', 'U'): U' R U' R' F' U' F | U2 R' F R F2 U' F | U' F2 L F L' U2 F | U2 F' U L' U2 L F
('R', 'D', 'F'), ('U', 'F'): U2 R B U2 B' U R' | U' F' U2 F R U2 R' | U' R U2 B' R B R2 | U' R U' R2 F R F'
('R', 'D', 'F'), ('F', 'U'): F' U' F U F' U' F | U2 R U' R' F' U' F | U R' F R F2 U' F | U2 F2 L F L' U2 F | U F' U L' U2 L F
('R', 'D', 'F'), ('U', 'L'): R U B U B' U R' | U R B U2 B' U R' | R B' R B R' U2 R' | U2 F' U2 F R U2 R' | L U' R U L' U2 R' | U2 R U2 B' R B R2 | U2 R U' R2 F R F'
('R', 'D', 'F'), ('B', 'L'): F' U F' D' L2 D F2
('R', 'D', 'F'), ('B', 'R'): R2 B U B' R2 U' R2 | F2 D' F' D F' U' R2 | F' U B U B' U F | B U' F' U2 B' U F
('R', 'D', 'F'), ('R', 'B'): R' U2 F R F2 U' F
('R', 'D', 'F'), ('L', 'F'): R U' R2 D' F' D R
('D', 'F', 'R'), ('B', 'U'): U2 F' U F2 R' F' R | F' U' L' U' L U' F | U' F' L' U2 L U' F | U2 R U2 R' F' U2 F | F' L F' L' F U2 F | B' U F' U' B U2 F | U2 F' U2 L F' L' F2
('D', 'F', 'R'), ('U', 'R'): R U R' U' R U R' | U2 F' U F R U R' | U' F R' F' R2 U R' | U2 R2 B' R' B U2 R' | U' R U' B U2 B' R'
('D', 'F', 'R'), ('R', 'U'): U F' U F2 R' F' R | U2 F' L' U2 L U' F | U R U2 R' F' U2 F | U F' U2 L F' L' F2
('D', 'F', 'R'), ('U', 'F'): U F' U F R U R' | U2 F R' F' R2 U R' | U R2 B' R' B U2 R' | U2 R U' B U2 B' R'
('D', 'F', 'R'), ('B', 'L'): R U' R D B2 D' R2
('D', 'F', 'R'), ('R', 'B'): F' U F2 D R D' F'
('D', 'F', 'R'), ('F', 'L'): R U' L' U' L U' R' | L' U R U2 L U' R' | R2 D R D' R U F2 | F2 L' U' L F2 U F2
('D', 'F', 'R'), ('L', 'F'): F U2 R' F' R2 U R'
('L', 'F', 'D'), ('U', 'B'): U L' U L R U' R' | R U L' U L U' R' | U L' U' L R U R' | U L' U' R U L R' | R' F' R2 U R2 F R
('L', 'F', 'D'), ('B', 'U'): F U F' U2 F' U' F | F U' F' U2 F' U F | F U' F' U F' U2 F
('L', 'F', 'D'), ('R', 'U'): R D2 B U B' D2 R'
('L', 'F', 'D'), ('U', 'F'): U' L' U L R U' R' | R U' L' U L U' R' | U' L' U' L R U R' | U' L' U' R U L R'
('L', 'F', 'D'), ('F', 'U'): F U2 F' U F' U' F | F U F U' F U F2 | L F L' F L F' L'
('L', 'F', 'D'), ('U', 'L'): U2 L' U L R U' R' | R U2 L' U L U' R' | L' U2 L U R U R' | U2 L' U' L R U R' | U2 L' U' R U L R' | L F2 L' U' F2 U F2
('L', 'F', 'D'), ('B', 'L'): R D' L U2 L' D R' | L2 F2 L2 U' F2 U F2 | L2 U' L2 U F2 L2 F2 | L F2 L' D' L2 D F2 | L' F' L2 F' L F2 L | L' F' L F' L2 F2 L | R2 D' B2 D R2 U' L2
('L', 'F', 'D'), ('B', 'R'): L' R' U L R2 U' R' | F2 D R2 D' R' F2 R | F2 R2 U R2 U' F2 R2 | F2 D R2 D' R2 F2 R2 | L' U' L B' R2 B R2 | L' B' U' R2 B L R2 | L' U' B' R2 B L R2 | B F U B' F2 U F | R2 U' L2 D B2 D' L2
('F', 'D', 'L'), ('R', 'U'): F U' F' U' F' U' F | U2 L' U' L F' U' F | U' L' U' F' U' F L
('F', 'D', 'L'), ('F', 'U'): L F2 L' F' R' F' R | U L' U' L F' U' F | F U F2 L F' L' F2 | L F2 D F' D' F' L' | U2 L' U' F' U' F L
('F', 'D', 'L'), ('L', 'B'): L' U' L2 F' U' L' F | F U F2 L U' L' F | L' U' L2 F' U' F L' | F U F2 L U' F L'
('F', 'D', 'L'), ('R', 'B'): R F R U' F' U' R' | F D R2 U' R' D' F'
('F', 'D', 'L'), ('L', 'F'): F L F' L' F2 U' F | F U' F2 R' F' R F
('D', 'L', 'F'), ('U', 'B'): L' U' R U' L U' R' | L' U L U' R U R' | L F2 L2 U' L U F2
('D', 'L', 'F'), ('U', 'R'): F U2 F' U R U R' | F U' R U R2 F' R | F2 U R U' R2 F2 R | F2 U2 R U2 R2 F2 R | F2 R2 D R D' F2 R | R2 U2 F2 R' F2 U2 R2 | R' F2 R2 U2 R' U2 F2 | L F2 L2 U2 L U2 F2 | F2 U2 L' U2 L2 F2 L' | L2 D2 B2 R' B2 D2 L2
('D', 'L', 'F'), ('U', 'F'): F U F' U2 R U R' | L F' L2 U L U' F | L F' D F' D' L' F2 | L F2 D' L D L2 F2
('D', 'L', 'F'), ('U', 'L'): R2 D2 B2 L' B2 D2 R2 | L2 U2 F2 L' F2 U2 L2
('D', 'L', 'F'), ('B', 'L'): F2 R' D2 L' D2 F2 R | F2 R' D2 L2 D2 R F2 | F2 L' F2 L2 F2 L F2
('D', 'L', 'F'), ('F', 'R'): R2 U' F U' F' U2 R2 | F' U F2 U2 F2 U F | F' U F2 U F2 U2 F
###################################################################### 154 cases can be done in 7 moves
568.1183922000346 s
###################################################################### So far we have 316/383 cases completed
('B', 'U', 'L'), ('U', 'B'): U' F' U2 F U' R U R' | U2 R U' B U2 B' U2 R' | U2 R' U2 R2 U R2 U R
('B', 'U', 'L'), ('F', 'U'): F2 L2 F' R' F L2 F2 R | U2 R2 U2 F R2 F' U2 R2 | F R2 F' U2 F' U2 F R2 | F R2 B' R2 F' R2 B R2 | B U2 B' U2 B' R2 B R2 | U' F' U2 L' U L U' F | U2 F' U' L' U2 L U' F | U' F' U2 F U2 F' U F | U' F' U2 F U F' U2 F | U F' U' L' U2 L U2 F | F2 L2 F L2 B' U2 B F | B' F' U2 B L2 F' L2 F2 | U2 L' B2 R2 B R2 B L | U2 B' R' U' R2 U R' B
('B', 'U', 'L'), ('L', 'U'): U2 R U' B U' B' U2 R' | U2 R2 B' D B' D' B2 R2 | U2 R U' R' U2 F' U' F | U2 F' U2 L' U' L U' F | R U R' F' L' U2 L F | U2 F' L' B' U B L F
('U', 'L', 'B'), ('U', 'B'): U2 F' U F U2 R U R' | U2 R U2 B U B' U R' | F' U' F R B U2 B' R' | U2 R B L U' L' B' R' | U2 F' U L' U L U2 F | U2 F2 L D' L D L2 F2
('U', 'L', 'B'), ('U', 'R'): U R U2 R' U2 R U' R' | U R U2 B U' B' U R' | U2 R U B U2 B' U R' | U R U2 R' U' R U2 R' | U' R U B U2 B' U2 R' | R2 B2 R' B2 L U2 L' R' | L R U2 L' B2 R B2 R2 | R2 B2 R F R' B2 R2 F' | U2 F2 U2 R' F2 R U2 F2 | R' F2 R U2 R U2 R' F2 | R' F2 L F2 R F2 L' F2 | L' U2 L U2 L F2 L' F2 | U2 L F U F2 U' F L' | U2 B L2 F2 L' F2 L' B'
('U', 'L', 'B'), ('L', 'U'): U2 F U2 F2 U' F2 U' F' | U R U2 R' U F' U' F | U2 F' U L' U2 L U2 F
('B', 'R', 'U'), ('U', 'B'): U R U2 R' U' R U R' | U' R2 U2 F R F' U2 R2 | U' R2 B2 L' B' L B' R2
('B', 'R', 'U'), ('B', 'U'): U' R B U B2 R B R2 | R B' R B R F R F' | R' F R F' U' F' U' F | U' F2 U2 F U F' U F2
('B', 'R', 'U'), ('U', 'R'): U' R2 U2 R' U' R U' R2 | R B U' B' R' F' U' F | F' U' L' U' L2 F' L' F2 | U' F' L' U' L2 F' L' F2 | F' U2 F' U' L' U L F2
('B', 'R', 'U'), ('R', 'U'): U F' U2 F U F' U' F | U' F2 U2 R' F' R U2 F2 | U' F2 L2 B L B' L F2
('R', 'U', 'B'), ('B', 'U'): U R U' B U' B' U2 R' | U R2 B' D B' D' B2 R2 | F R' F' R U' F' U' F | U R U' R' U2 F' U' F | U F' U2 L' U' L U' F | R2 B' R2 F' R B R' F | U F' L' B' U B L F
('R', 'U', 'B'), ('U', 'R'): U2 F' U2 F U' R U R' | U R U' B U2 B' U2 R' | U R' U2 R2 U R2 U R
('R', 'U', 'B'), ('R', 'F'): U' F' U' F U R U' R' | U2 F' U' F U' R U R' | U' F' U F U R U R' | U' F' U' F U2 R U2 R' | U' F' U' F R' F R F' | U2 F' L F' L' F U' F | F2 U' L' U L F U2 F | F' U2 F' L' U' L U F2
('U', 'B', 'R'), ('U', 'R'): U F' U F U2 R U R' | F' L' U' L F R U R' | U R U2 B U B' U R' | F' U2 F R B U2 B' R' | U R B L U' L' B' R' | U F' U L' U L U2 F | F' R' F' U' F U R F | R2 D R D' R F2 U F2 | U F2 L D' L D L2 F2
('U', 'B', 'R'), ('R', 'U'): U2 F' U F U' F' U' F | U' F2 U' F U' F' U2 F2 | U2 F2 D' F U' F' D F2
('R', 'F', 'U'), ('R', 'U'): U2 R B U B2 R B R2 | F' U' F' L F L' U2 F | U2 F2 U2 F U F' U F2
('R', 'F', 'U'), ('U', 'F'): R U R B' R' B U2 R' | U2 R2 U2 R' U' R U' R2 | U2 F' L' U' L2 F' L' F2
('F', 'U', 'R'), ('U', 'R'): U' R U' R' U R U R' | F' U' F2 R' F' R2 U R' | R B L' B L B2 U2 R' | U2 R2 U R' U R U2 R2 | R B' U' R' U R2 B R2 | F R B' R' F' R2 B R2 | F' R B' R' F R2 B R2 | U' R2 D R' U R D' R2 | F' U L' U L2 F' L' F2
('F', 'U', 'R'), ('R', 'F'): U2 F' U' F U R U' R' | U F' U' F U' R U R' | U2 F' U F U R U R' | U2 F' U' F U2 R U2 R' | U2 F' U' F R' F R F' | U F' L F' L' F U' F
('U', 'R', 'F'), ('F', 'U'): R U' B U' B2 R B R2 | U F' U F U' F' U' F | R U R2 F R F2 U' F | F' L' B L' B' L2 U2 F | U2 F2 U' F U' F' U2 F2 | F' L U F U' F2 L' F2 | R F' L F R' F2 L' F2 | R' F' L F R F2 L' F2 | U F2 D' F U' F' D F2
('U', 'R', 'F'), ('R', 'F'): U' R B' R B R' U R' | U2 R U R' F R' F' R | U2 R U' R' U' F' U' F | U' R U R' U F' U' F | U2 R U R' U' F' U F | U2 R U R' U2 F' U2 F
('F', 'L', 'U'), ('U', 'F'): U' R U2 R' U' R U R' | U R2 U2 F R F' U2 R2 | U R2 B2 L' B' L B' R2
('F', 'L', 'U'), ('F', 'U'): F' L' U L F R U R' | R U2 R U B U' B' R2 | R U B U B2 R B R2 | U R B U B2 R B R2 | U F2 U2 F U F' U F2
('F', 'L', 'U'), ('U', 'L'): F R' F' R U R U R' | F' L F' L' F' R' F' R | U R2 U2 R' U' R U' R2 | U F' L' U' L2 F' L' F2
('F', 'L', 'U'), ('L', 'U'): U' F' U2 F U F' U' F | U F2 U2 R' F' R U2 F2 | U F2 L2 B L B' L F2
('L', 'U', 'F'), ('U', 'F'): U2 R U' R' U R U R' | U R2 U R' U R U2 R2 | U2 R2 D R' U R D' R2
('L', 'U', 'F'), ('F', 'U'): U' R U' B U' B' U2 R' | R F R U R' U' F' R' | F2 D' F' D F' R2 U' R2 | U' R2 B' D B' D' B2 R2 | U' R U' R' U2 F' U' F | R B U B' R' F' U' F | U' F' U2 L' U' L U' F | R U2 R' F' L' U2 L F | U' F' L' B' U B L F
('U', 'F', 'L'), ('F', 'U'): U' F U2 F2 U' F2 U' F' | U2 R U2 R' U F' U' F | U' F' U L' U2 L U2 F
('U', 'F', 'L'), ('U', 'L'): R' F R F' U R U R' | U' F' U F U2 R U R' | U' R U2 B U B' U R' | F2 L F2 R F' L' F R' | U' R B L U' L' B' R' | U' F' U L' U L U2 F | U' F2 L D' L D L2 F2
('U', 'F', 'L'), ('R', 'F'): U2 R B' R B R' U R' | R2 U B U' B' R' U2 R' | U R U R' F R' F' R | R U2 R B U B' U' R2 | U R U' R' U' F' U' F | U2 R U R' U F' U' F | U R U R' U' F' U F | U R U R' U2 F' U2 F
('B', 'L', 'D'), ('L', 'B'): R B' U B2 U2 B' U2 R' | F' L U' L2 U2 L U2 F | L U2 F U2 F2 U' F L' | B' U2 R' U2 R2 U R' B
('B', 'L', 'D'), ('R', 'F'): B' F' U B F R U2 R' | L R U' L' R' F' U2 F | L2 F' L' U F' L F2 L2 | L F2 L F2 D F' D' L2 | B2 R B U' R B' R2 B2 | B' R2 B' R2 D' R D B2
('L', 'D', 'B'), ('B', 'L'): R' D' R2 U' R D B2 R2 | B2 R2 D B U' B2 D' B'
('L', 'D', 'B'), ('L', 'B'): B2 R B R' B R U R' | B' U B R B' R B R2 | D2 F' L U2 L' U F D2 | D2 F' U' L U' L' F D2 | D2 L F' U' F U' L' D2 | D2 L U F' U2 F L' D2
('L', 'D', 'B'), ('R', 'F'): L2 D F' D' L2 R U' R' | R U' L' B L B' U2 R' | F' U F R B' U2 B R' | R B U' B2 U R B R2 | R B2 U' B U R B2 R2 | F' U L2 F' L' F L' F | L2 D F D' F2 L' F2 L' | L U2 F' U F' L F2 L2 | F' U B' F R U2 R' B | B2 R2 B U R U' R2 B
('L', 'D', 'B'), ('L', 'F'): L' U B L B' R U2 R' | L' R U B L B' U2 R' | L' U R B L B' U2 R' | R B' U2 B F U' F' R' | R B' U2 F U' B F' R' | R F U B' F' U2 B R' | F U F' R B' U2 B R' | R F U' F2 L2 F L2 R' | B' U2 B R' D' F' D R | L' U' L2 U2 F' L' U2 F | L U2 F' L2 U L U2 F | F' L2 F L' F' U' L' F | F' L2 F L' F' U' F L' | D' L D L F U' F2 L' | F U F2 L2 F' L' F2 L' | B' U2 B L' F' U' F L | L2 U' B' R U R' B L2 | F U B' F' R U2 R' B | B' R U2 F U' F' R' B | B' U2 R' D' F' D R B | F U B' F L F2 L' B | B' U2 L' F' U' F L B
('D', 'B', 'L'), ('B', 'L'): F D F2 U F' D' L2 F2 | L2 F2 D' L' U L2 D L
('D', 'B', 'L'), ('L', 'B'): L2 F' L' F L' F' U' F | L U' L' F' L F' L' F2 | D2 R B' U2 B U' R' D2 | D2 R U B' U B R' D2 | D2 B' R U R' U B D2 | D2 B' U' R U2 R' B D2
('D', 'B', 'L'), ('R', 'B'): B' U2 R B2 U' B' U2 R' | B U B2 U2 R B U2 R' | R B2 R' B R U B R' | L U2 L' F D R D' F' | B U' L' B' L F' U2 F | B F' U' L' B' L U2 F | B U' F' L' B' L U2 F | F' L U2 L' R' U R F | F' L U2 R' U L' R F | F' R' U' L R U2 L' F | R' U' R F' L U2 L' F | F' R' U R2 B2 R' B2 F | L U2 F D R D' F' L' | R' U' L R F' U2 F L' | L F' U2 R' U R F L' | L U2 B R U R' B' L' | R' U' L R' B' R2 B L' | L U2 L' B R U R' B' | R B2 R' B R U R' B | D B' D' B' R' U R2 B | R' U' R2 B2 R B R2 B | B2 U L F' U' F L' B2
('D', 'B', 'L'), ('R', 'F'): R U' B2 R B R' B R' | B2 D' R D B2 F' U F | F' U B L' B' L U2 F | R U' R' F' L U2 L' F | F' L' U L2 U' F' L' F2 | F' L2 U L' U' F' L2 F2 | R U' L R' F' U2 F L' | L2 F2 L' U' F' U F2 L' | B2 D' R' D R2 B R2 B | B' U2 R U' R B' R2 B2
('R', 'B', 'D'), ('U', 'B'): R2 B U2 B' U2 R' U' R' | U' R' U R U2 R U' R' | R2 F R2 F' U2 R U' R' | R D R' U2 R D' U' R' | U R' U2 R U' R U R' | U' R' U' R U2 R U R' | R' U F' U' F R2 U R' | R D B U' B' D' U R' | U' R' U R U' R U2 R' | R2 F R2 F' U' R U2 R' | R D R' U' R D' U2 R' | U R' U' R' U R' U' R2 | B U' B' R2 U R U' R2 | R2 F' U' F U2 R U' R2 | U2 F' D2 L' U' L D2 F | B U L F' U' F L' B' | U B' R' B R' B' R B
('R', 'B', 'D'), ('L', 'B'): R2 U2 B' U2 B R' U' R' | R2 U2 B' U2 R' B U' R' | R' U B' U' B R2 U2 R' | R2 U2 B' U2 R' U' B R' | B' R2 U R' B R2 U' R2 | B U L' B' L F' U' F | B U B' L F' U' F L' | R2 U2 B' U2 R' U' R' B | B' U R' B R' B' R B
('R', 'B', 'D'), ('R', 'F'): R' U B' R' B R2 U R' | B' R B U F R F' R'
('R', 'B', 'D'), ('L', 'F'): D' F2 U' F' D R' U' R' | D2 F U F' D2 R U2 R' | B' R B F R U' F' R' | B U B' R' D' F' D R | R' F' U F D' F' D R | R' B' R B D' F' D R | F R U' F' R' F R' F' | F' R' U F' U' R U F | D2 L' U2 L D2 F' U F | F' R2 F' R F2 U R F | B U B' L' F' U' F L | B U R' D' F' D R B' | B U L' F' U' F L B' | D' R U2 F U' F' R' D | D' L' F' U' F U2 L D
('B', 'D', 'R'), ('L', 'B'): R2 B' R U2 R2 B U' R' | R B' R2 U2 R2 B U' R' | B2 U2 B R U2 B2 U' R' | B' R2 U B U' R' U R' | B' R2 U2 B U2 R' U R' | R' B' U' R' B R2 U2 R' | B U2 B2 U' R B U2 R' | R D B' U' B' D' U2 R' | R D B D' F R2 F' R' | R2 B' R U2 R2 U' B R' | R B' R2 U2 R2 U' B R' | R2 B' R U' R2 U2 B R' | R B' R2 U' R2 U2 B R' | B' U' B2 R B2 U2 R' B' | R2 B' R U2 R2 U' R' B | R B' R2 U2 R2 U' R' B | R2 B' R U' R2 U2 R' B | R B' R2 U' R2 U2 R' B | B2 U2 B R U R' U B2
('B', 'D', 'R'), ('B', 'R'): B' L2 F R2 F' L2 B R2 | R2 F L2 B' R2 B L2 F'
('B', 'D', 'R'), ('L', 'F'): F U2 F' R' U2 R2 U' R' | F U2 R' F' U2 R2 U' R' | B' F2 R2 B F2 R' U R' | F U2 F' R' U' R2 U2 R' | F U2 R' F' U' R2 U2 R' | F U2 R' U' F' R2 U2 R' | R' U2 R2 F R' U' F' R' | R' U2 R2 U' F U' F' R' | R' U' R2 U2 F U' F' R' | R2 F2 R2 F' D R D' F'
('D', 'R', 'B'), ('B', 'R'): B2 U R' B2 R U' B2 R | B2 U2 R' B2 R U2 B2 R | R F2 U' R F2 R' U F2 | R F2 U2 R F2 R' U2 F2
('D', 'R', 'B'), ('R', 'F'): F' R' U2 R F R U R' | B' D' R2 D B R U R' | R' U R2 U F R F' R' | R' U2 R2 U2 R' F' U' F | F R' F' U R F' U F | B' D' R D B F' U F | F' U2 R B' R' B U F
('R', 'D', 'F'), ('R', 'F'): F' L' U2 L F R U R' | F' U F R B U2 B' R' | R U' R U B U' B' R2 | R U2 B U B2 R B R2 | F2 L' U' L U F U' F | F2 L F L2 U L U2 F
('D', 'F', 'R'), ('R', 'F'): R2 B U B' U' R' U R' | R2 B' R' B2 U' B' U2 R' | R B U2 B' R' F' U' F | R U' R' F' L' U2 L F | F' U2 L' U' L2 F' L' F2 | F' U F' U' L' U L F2
('L', 'F', 'D'), ('L', 'U'): U2 R D2 B U B' D2 R' | U' F U2 F' U F' U' F | U F U F' U2 F' U' F | F U' R U R' F2 U' F | F' D' L' U L D U' F | F2 R' F2 R U2 F' U F | U F U' F' U2 F' U F | F2 L' U2 L U2 F U F | F' D' F U2 F' D U F | F2 R' F2 R U F' U2 F | U F U' F' U F' U2 F | F' D' F U F' D U2 F | L' U L F2 U' F' U F2 | F2 R U R' U2 F' U F2 | U' F U F U' F U F2 | U' L F L' F L F' L' | L' U' B' R U R' B L
('L', 'F', 'D'), ('L', 'B'): L' U' B L B' R U R' | F2 U2 L U2 L' F U F | F2 U2 L U2 F L' U F | F U' L U L' F2 U2 F | F2 U2 L U2 F U L' F | L F2 U' F L' F2 U F2 | L U' F L' F L F' L' | F2 U2 L U2 F U F L' | L' U' L B' R U R' B
('L', 'F', 'D'), ('R', 'B'): D2 B U2 B' D2 R U' R' | R F U' R U F' U' R' | R F2 R F' R2 U' F' R' | R' F' U R F R' F R | F R U' R' D R D' F' | L' U' L F D R D' F' | F L F' L' D R D' F' | D R2 U R D' F U F | D2 R' U' R D2 F' U2 F | L F' L' R' F' U R F | L' U' F D R D' F' L | L' U' B R U R' B' L | L' U' L B R U R' B' | D F' U2 R' U R F D' | D B R U R' U2 B' D'
('L', 'F', 'D'), ('R', 'F'): F U' L F L' F2 U' F | L F' L' U' R' F' R F
('F', 'D', 'L'), ('R', 'F'): R' F R U' F' R U' R' | L D F' D' L' R U' R' | R U2 F' L F L' U' R' | F U2 F2 U2 F R U R' | R F U2 F' R' F' U' F | L D F2 D' L' F' U' F | F U' F2 U' R' F' R F
('F', 'D', 'L'), ('F', 'L'): F' R2 U F' R2 F U' R2 | F' R2 U2 F' R2 F U2 R2 | L2 U' F L2 F' U L2 F' | L2 U2 F L2 F' U2 L2 F'
('D', 'L', 'F'), ('L', 'B'): L F2 U' L' U F U' F | L F2 U2 L' U2 F U' F | F2 L F' U2 F2 L' U F | F' L F2 U2 F2 L' U F | L2 U2 L' F' U2 L2 U F | F L U F L' F2 U2 F | L' U2 L2 U F' L' U2 F | F' D' L U L D U2 F | F' D' L' D R' F2 R F | F2 L F' U2 F2 U L' F | F' L F2 U2 F2 U L' F | F2 L F' U F2 U2 L' F | F' L F2 U F2 U2 L' F | F2 L F' U2 F2 U F L' | F' L F2 U2 F2 U F L' | F2 L F' U F2 U2 F L' | F' L F2 U F2 U2 F L' | L U L2 F' L2 U2 F L | L2 U2 L' F' U' F U' L2
('D', 'L', 'F'), ('R', 'B'): F2 R2 F2 R D' F' D R | L R2 F2 L' R2 F U' F | R' U2 F R U2 F2 U F | R' U2 R F U2 F2 U F | R' U2 F R U F2 U2 F | R' U2 R F U F2 U2 F | R' U2 F U R F2 U2 F | F U2 F2 U R' U R F | F U F2 U2 R' U R F | F U2 F2 R' F U R F
('D', 'L', 'F'), ('F', 'L'): F2 R' B2 L F2 L' B2 R | L B2 R' F2 R B2 L' F2
###################################################################### 60 cases can be done in 8 moves
7323.567334699968 s
###################################################################### So far we have 376/383 cases completed
'''
