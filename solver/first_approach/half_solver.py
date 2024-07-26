# OK, we need a 1., 2., 3., and 4.
# Step 1. Solve the first (top) layer PROPERLY.
#   - put middle whites in exact places it should be in.
#   - put corner whites in exact places it should be in.
import numpy as np



#####################################  CUBE DATA STRUCTURE(S)  ##########################################
# But first, lets get our 2nd data structure type. 
# yellow is top and orange is front
CUBE = {'U': np.array([['R', 'F', 'R'], 
                      ['U', 'U', 'R'], 
                      ['U', 'B', 'D']]),

        'R': np.array([['L', 'B', 'U'], 
                      ['D', 'R', 'F'], 
                      ['B', 'U', 'R']]),

        'F': np.array([['B', 'D', 'F'], 
                      ['D', 'F', 'L'], 
                      ['B', 'B', 'L']]),

        'L': np.array([['B', 'B', 'R'], 
                      ['R', 'L', 'R'], 
                      ['U', 'U', 'D']]),  

        'B': np.array([['F', 'D', 'D'], 
                      ['U', 'B', 'F'], 
                      ['D', 'F', 'L']]),

        'D': np.array([['F', 'L', 'F'], 
                      ['R', 'D', 'L'], 
                      ['U', 'L', 'L']])}

F_MOVES = []



#################################  GENERIC OPERATION CORRESPONDANCES  #######################################
#                       (face, move) -> (front, move)
move_from_Fs_perspective = {('F', 'F'): ('F', 'F'),
                            ('F', 'U'): ('F', 'U'),
                            ('F', 'R'): ('F', 'R'),
                            ('F', 'D'): ('F', 'D'),
                            ('F', 'L'): ('F', 'L'),
                            ('F', 'B'): ('F', 'B'),

                            ('U', 'F'): ('F', 'U'),
                            ('U', 'U'): ('F', 'B'),
                            ('U', 'R'): ('F', 'R'),
                            ('U', 'D'): ('F', 'F'),
                            ('U', 'L'): ('F', 'L'),
                            ('U', 'B'): ('F', 'D'),
                            
                            ('R', 'F'): ('F', 'R'),
                            ('R', 'U'): ('F', 'U'),
                            ('R', 'R'): ('F', 'B'),
                            ('R', 'D'): ('F', 'D'),
                            ('R', 'L'): ('F', 'F'),
                            ('R', 'B'): ('F', 'L'),
                            
                            ('D', 'F'): ('F', 'D'),
                            ('D', 'U'): ('F', 'B'),
                            ('D', 'R'): ('F', 'L'),
                            ('D', 'D'): ('F', 'F'),
                            ('D', 'L'): ('F', 'R'),
                            ('D', 'B'): ('F', 'U'),
                            
                            ('L', 'F'): ('F', 'L'),
                            ('L', 'U'): ('F', 'U'),
                            ('L', 'R'): ('F', 'F'),
                            ('L', 'D'): ('F', 'D'),
                            ('L', 'L'): ('F', 'B'),
                            ('L', 'B'): ('F', 'R'),
                            
                            ('B', 'F'): ('F', 'B'),
                            ('B', 'U'): ('F', 'U'),
                            ('B', 'R'): ('F', 'L'),
                            ('B', 'D'): ('F', 'D'),
                            ('B', 'L'): ('F', 'R'),
                            ('B', 'B'): ('F', 'F')}
TRAIN1 = np.array([(0, 0), (0, 2), (2, 2), (2, 0)])
TRAIN2 = np.array([(0, 1), (1, 2), (2, 1), (1, 0)])
TRAINS345 = {'F': np.array([[('U', 2, 0), ('R', 0, 0), ('D', 2, 0), ('L', 2, 2)],
                            [('U', 2, 1), ('R', 1, 0), ('D', 2, 1), ('L', 1, 2)],
                            [('U', 2, 2), ('R', 2, 0), ('D', 2, 2), ('L', 0, 2)]]),

             'U': np.array([[('B', 0, 2), ('R', 0, 2), ('F', 0, 2), ('L', 0, 2)],
                            [('B', 0, 1), ('R', 0, 1), ('F', 0, 1), ('L', 0, 1)],
                            [('B', 0, 0), ('R', 0, 0), ('F', 0, 0), ('L', 0, 0)]]),

             'R': np.array([[('U', 2, 2), ('B', 0, 0), ('D', 0, 0), ('F', 2, 2)],
                            [('U', 1, 2), ('B', 1, 0), ('D', 1, 0), ('F', 1, 2)],
                            [('U', 0, 2), ('B', 2, 0), ('D', 2, 0), ('F', 0, 2)]]),

             'D': np.array([[('B', 2, 0), ('L', 2, 0), ('F', 2, 0), ('R', 2, 0)],
                            [('B', 2, 1), ('L', 2, 1), ('F', 2, 1), ('R', 2, 1)],
                            [('B', 2, 2), ('L', 2, 2), ('F', 2, 2), ('R', 2, 2)]]),

             'L': np.array([[('U', 0, 0), ('F', 0, 0), ('D', 2, 2), ('B', 2, 2)],
                            [('U', 1, 0), ('F', 1, 0), ('D', 1, 2), ('B', 1, 2)],
                            [('U', 2, 0), ('F', 2, 0), ('D', 0, 2), ('B', 0, 2)]]),

             'B': np.array([[('U', 0, 2), ('L', 0, 0), ('D', 0, 2), ('R', 2, 2)],
                            [('U', 0, 1), ('L', 1, 0), ('D', 0, 1), ('R', 1, 2)],
                            [('U', 0, 0), ('L', 2, 0), ('D', 0, 0), ('R', 0, 2)]])}

faces_to_nums = {'F': 0,
                 'L': 1,
                 'B': 2,
                 'R': 3}




######################################  SPECIFIC TO MIDDLE PIECES  ###############################################
middle_references = {('U', 0, 1): ('B', 0, 1),
                    ('U', 1, 0): ('L', 0, 1),
                    ('U', 1, 2): ('R', 0, 1),
                    ('U', 2, 1): ('F', 0, 1),

                    ('L', 0, 1): ('U', 1, 0),
                    ('L', 1, 0): ('B', 1, 2),
                    ('L', 1, 2): ('F', 1, 0),
                    ('L', 2, 1): ('D', 1, 0),
                    
                    ('F', 0, 1): ('U', 2, 1),
                    ('F', 1, 0): ('L', 1, 2),
                    ('F', 1, 2): ('R', 1, 0),
                    ('F', 2, 1): ('D', 2, 1),
                    
                    ('D', 0, 1): ('B', 2, 1),
                    ('D', 1, 0): ('R', 2, 1),
                    ('D', 1, 2): ('L', 2, 1),
                    ('D', 2, 1): ('F', 2, 1),
                    
                    ('R', 0, 1): ('U', 1, 2),
                    ('R', 1, 0): ('F', 1, 2),
                    ('R', 1, 2): ('B', 1, 0),
                    ('R', 2, 1): ('D', 1, 0),
                    
                    ('B', 0, 1): ('U', 0, 1),
                    ('B', 1, 0): ('R', 1, 2),
                    ('B', 1, 2): ('L', 1, 0),
                    ('B', 2, 1): ('D', 0, 1)}
                        # (rel0, rel1): algorithm
bottom_cross_algorithms = {('F', 'U'): "F, D, R', D'",
                           ('F', 'R'): "D, R', D'",
                           ('F', 'D'): "F, D', L, D",
                           ('F', 'L'): "D', L, D",
                           ('U', 'B'): "U2, F2",
                           ('U', 'R'): "U, F2",
                           ('U', 'F'): "F2",
                           ('U', 'L'): "U', F2",
                           ('R', 'U'): "R', F, R",
                           ('R', 'B'): "D2, B', D2",
                           ('R', 'D'): "R, F",
                           ('R', 'F'): "F",
                           ('D', 'B'): "B2, U2, F2",
                           ('D', 'L'): "L2, U', F2",
                           ('D', 'R'): "R2, U, F2",
                           ('L', 'U'): "L, F', L'",
                           ('L', 'F'): "F'",
                           ('L', 'D'): "L', F'",
                           ('L', 'B'): "D2, B, D2",
                           ('B', 'U'): "U, R', F, R",
                           ('B', 'L'): "D', L', D",
                           ('B', 'D'): "B, D, R, D'",
                           ('B', 'R'): "D, R, D'"}




########################################  FUNCTIONS  ###########################################################
def update_cube_with_F_move(cube, F_move: str, prime: bool) -> None:
    TRAIN3, TRAIN4, TRAIN5 = TRAINS345[F_move]
    if prime:
        for T in (TRAIN1, TRAIN2):
            a1 = cube[F_move][T[1][0], T[1][1]]
            a2 = cube[F_move][T[2][0], T[2][1]]
            a3 = cube[F_move][T[3][0], T[3][1]]
            a4 = cube[F_move][T[0][0], T[0][1]]
            cube[F_move][T[0][0], T[0][1]] = a1
            cube[F_move][T[1][0], T[1][1]] = a2
            cube[F_move][T[2][0], T[2][1]] = a3
            cube[F_move][T[3][0], T[3][1]] = a4
        for T in (TRAIN3, TRAIN4, TRAIN5):
            a1 = cube[T[1][0]][int(T[1][1]), int(T[1][2])]
            a2 = cube[T[2][0]][int(T[2][1]), int(T[2][2])]
            a3 = cube[T[3][0]][int(T[3][1]), int(T[3][2])]
            a4 = cube[T[0][0]][int(T[0][1]), int(T[0][2])]
            cube[T[0][0]][int(T[0][1]), int(T[0][2])] = a1
            cube[T[1][0]][int(T[1][1]), int(T[1][2])] = a2
            cube[T[2][0]][int(T[2][1]), int(T[2][2])] = a3
            cube[T[3][0]][int(T[3][1]), int(T[3][2])] = a4
    else:
        for T in (TRAIN1, TRAIN2):
            a1 = cube[F_move][T[3][0], T[3][1]]
            a2 = cube[F_move][T[0][0], T[0][1]]
            a3 = cube[F_move][T[1][0], T[1][1]]
            a4 = cube[F_move][T[2][0], T[2][1]]
            cube[F_move][T[0][0], T[0][1]] = a1
            cube[F_move][T[1][0], T[1][1]] = a2
            cube[F_move][T[2][0], T[2][1]] = a3
            cube[F_move][T[3][0], T[3][1]] = a4
        for T in (TRAIN3, TRAIN4, TRAIN5):
            a1 = cube[T[3][0]][int(T[3][1]), int(T[3][2])]
            a2 = cube[T[0][0]][int(T[0][1]), int(T[0][2])]
            a3 = cube[T[1][0]][int(T[1][1]), int(T[1][2])]
            a4 = cube[T[2][0]][int(T[2][1]), int(T[2][2])]
            cube[T[0][0]][int(T[0][1]), int(T[0][2])] = a1
            cube[T[1][0]][int(T[1][1]), int(T[1][2])] = a2
            cube[T[2][0]][int(T[2][1]), int(T[2][2])] = a3
            cube[T[3][0]][int(T[3][1]), int(T[3][2])] = a4


def execute_and_update_F_MOVES(cube, face: str, algorithm: str) -> None:
    new_moves = []
    for move in algorithm.split(", "):
        prime: bool = (move[-1]=="'")
        double: bool = (move[-1]=="2")
        mv = move[0]
        Omove = move_from_Fs_perspective[(face, mv)][1]
        update_cube_with_F_move(cube, Omove, prime)
        if double:
            update_cube_with_F_move(cube, Omove, prime)
            new_moves.append(f"{Omove}2")
        elif prime:
            new_moves.append(f"{Omove}'")
        else: new_moves.append(Omove)
    #print(new_moves)
    F_MOVES.extend(new_moves)

def find_a_misplaced_bottom_edge(cube) -> tuple[str, int, int]:
    for face, value in cube.items():
        if face == 'D':
            if value[0, 1] == 'D' and cube['B'][0, 1] != 'B': return (face, 0, 1)
            if value[1, 0] == 'D' and cube['L'][0, 1] != 'L': return (face, 1, 0)
            if value[1, 2] == 'D' and cube['R'][0, 1] != 'R': return (face, 1, 2)
            if value[2, 1] == 'D' and cube['F'][0, 1] != 'F': return (face, 2, 1)
        else:
            if value[0, 1] == 'D': return (face, 0, 1)
            if value[1, 0] == 'D': return (face, 1, 0)
            if value[1, 2] == 'D': return (face, 1, 2)
            if value[2, 1] == 'D': return (face, 2, 1)

def move_bottom_edge(cube, position: tuple[str, int, int]) -> None:
    (face0, x0, y0) = position
    # Find color to reunify
    (face1, x1, y1) = middle_references[(face0, x0, y0)]
    destination_face = cube[face1][x1, y1]

    # we will face the destination_face and our if conditions 
    # will use a tuple of the two faces relative to it.
    # for example, if destination_face was red, face0 was blue, and face1 was white, 
    #         then the tuple would be ('L', 'D')
    if face0 == 'U': rel0 = 'U'
    elif face0 == 'D': rel0 = 'D'
    else:
        relation0 = (faces_to_nums[face0] - faces_to_nums[destination_face])%4
        if relation0 == 0: rel0 = 'F'
        if relation0 == 1: rel0 = 'L'
        if relation0 == 2: rel0 = 'B'
        if relation0 == 3: rel0 = 'R'
        if face0 == 'U': rel0 = 'U'
    if face1 == 'U': rel1 = 'U'
    elif face1 == 'D': rel1 = 'D'
    else:
        relation1 = (faces_to_nums[face1] - faces_to_nums[destination_face])%4
        if relation1 == 0: rel1 = 'F'
        if relation1 == 1: rel1 = 'L'
        if relation1 == 2: rel1 = 'B'
        if relation1 == 3: rel1 = 'R'
    execute_and_update_F_MOVES(cube, destination_face, bottom_cross_algorithms[(rel0, rel1)])
        
def white_cross(cube):
    for _ in range(4):
        pos = find_a_misplaced_bottom_edge(cube)
        if pos is None: return
        move_bottom_edge(cube, pos)

def printcube(cube):
    for key, value in cube.items():
        print(f'{key}: ')
        print(value)
    print('-------------------------------------')

def simplify_F_MOVES(F_MOVES) -> list[str]:
    while True:
        new = []
        modified = False
        i = 0
        while i < len(F_MOVES):
            if i >= len(F_MOVES)-1: 
                new.append(F_MOVES[i])
                i += 1
                continue
            v1 = F_MOVES[i]
            v2 = F_MOVES[i+1]
            if v1[0] == v2[0]:
                modified = True
                count = 0
                if v1[-1] == "'": count -= 1
                if v1[-1] == "2": count += 2
                if v2[-1] == "'": count -= 1
                if v2[-1] == "2": count += 2
                if count == -2: new.append(f"{v1[0]}2")
                if count == -1: new.append(f"{v1[0]}'")
                if count == 1: new.append(f"{v1[0]}")
                if count == 2: new.append(f"{v1[0]}2")
                if count == 3: new.append(f"{v1[0]}'")
                i += 2
            else: 
                new.append(v1)
                i += 1
        F_MOVES = new[:]
        new = []
        i = 0
        while i < len(F_MOVES):
            if i >= len(F_MOVES)-2: 
                new.append(F_MOVES[i])
                i += 1
                continue
            v1 = F_MOVES[i]
            v2 = F_MOVES[i+1]
            v3 = F_MOVES[i+2]
            if v1[0] == v3[0] and (v1[0], v2[0]) in {('U', 'D'), ('D', 'U'), ('F', 'B'), ('B', 'F'), ('R', 'L'), ('L', 'R')}:
                modified = True
                count = 0
                if v1[-1] == "'": count -= 1
                if v1[-1] == "2": count += 2
                if v3[-1] == "'": count -= 1
                if v3[-1] == "2": count += 2
                if count == -2: new.append(f"{v1[0]}2")
                if count == -1: new.append(f"{v1[0]}'")
                if count == 1: new.append(f"{v1[0]}")
                if count == 2: new.append(f"{v1[0]}2")
                if count == 3: new.append(f"{v1[0]}'")
                new.append(v2)
                i += 3
            else: 
                new.append(v1)
                i += 1
        F_MOVES = new[:]
        if not modified: return F_MOVES

'''
NEXT: IMPLEMENT F2L
To identify a case, we will use a tuple of the relative faces 
for the front, right, and down stickers of the corner piece. 
In addition to that, we will use a tuple of the relative faces 
for the front and right stickers of the edge piece.
##########################################################
('R', 'F', 'U'), ('U', 'B'): "U, R, U2, R', U, R, U', R'"               | "U (R U2 R') U (R U' R')"
('R', 'F', 'U'), ('U', 'R'): "R, U2, R', U', R, U, R'"                  | "(R U2 R') U' (R U R')"
('R', 'F', 'U'), ('U', 'F'): "U, F, R', F', R, U, R, U, R'"             | "U (F R' F' R) U (R U R')"
('R', 'F', 'U'), ('U', 'L'): "R, B, U2, B', R'"                         | "y F R U2 R' F'"

('R', 'F', 'U'), ('B', 'U'): "F', L', U2, L, F"                         | "F' L' U2 L F"
('R', 'F', 'U'), ('R', 'U'): "F, U, R, U', R', F', R, U', R'"           | "y U' (F' L F L') U' (L' U' L)"
('R', 'F', 'U'), ('F', 'U'): "F', U2, F, U, F', U', F"                  | "y (L' U2 L) U (L' U' L)"
('R', 'F', 'U'), ('L', 'U'): "U', F', U2, F, U', F', U, F"              | "y U' (L' U2 L) U' (L' U L)"
##########################################################
('F', 'U', 'R'), ('U', 'B'): "R, U, R'"                                 | "(R U R')"
('F', 'U', 'R'), ('U', 'R'): "U', R, U', R', U, R, U, R'"               | "U' (R U' R') U (R U R')"
('F', 'U', 'R'), ('U', 'F'): "R', U2, R2, U, R2, U, R"                  | "R' U2 R2 U R2' U R"
('F', 'U', 'R'), ('U', 'L'): "U', R, U, R', U, R, U, R'"                | "U' (R U R') U (R U R')"

('F', 'U', 'R'), ('B', 'U'): "L, B2, R2, B, R2, B, L'"                  | "r' U2 R2 U R2' U r"
('F', 'U', 'R'), ('R', 'U'): "R, U', R', U2, F', U', F"                 | "(R U' R') y U2 (L' U' L)"
('F', 'U', 'R'), ('F', 'U'): "U', F', U, F"                             | "y U' (L' U L)"
('F', 'U', 'R'), ('L', 'U'): "U, B', U', B, U2, B', U, B"               | "y U (L' U' L) U2 (L' U L)"
##########################################################
('U', 'R', 'F'), ('U', 'B'): "U', R, U, R', U2, R, U', R'"              | "U' (R U R') U2 (R U' R')"
('U', 'R', 'F'), ('U', 'R'): "U, R, U', R'"                             | "U (R U' R')"
('U', 'R', 'F'), ('U', 'F'): "F', U, F, U2, R, U, R'"                   | "y (L' U L) y' U2 (R U R')"
('U', 'R', 'F'), ('U', 'L'): "B, L2, F2, L', F2, L', B'"                | "l U2 L2' U' L2 U' l'"

('U', 'R', 'F'), ('B', 'U'): "U, F', U', F, U', F', U', F"              | "y U (L' U' L) U' (L' U' L)"
('U', 'R', 'F'), ('R', 'U'): "F, U2, F2, U', F2, U', F'"                | "L U2 L2' U' L2 U' L'"
('U', 'R', 'F'), ('F', 'U'): "U, F', U, F, U', F', U', F"               | "y U (L' U L) U' (L' U' L)"
('U', 'R', 'F'), ('L', 'U'): "F', U', F"                                | "y (L' U' L)"
##########################################################
('F', 'U', 'R'), ('F', 'R'): "U, R, U, R', U2, R, U, R'"                | "U (R U R') U2 (R U R')"
('R', 'F', 'U'), ('F', 'R'): "U, R, U', R', U, R, U', R', U, R, U', R'" | "(U R U' R')3"
('U', 'R', 'F'), ('F', 'R'): "U', R, U', R', U2, R, U', R'"             | "U' R U' R' U2 R U' R'"

('F', 'U', 'R'), ('R', 'F'): "U, F', U', F, U', R, U, R'"               | "U (F' U' F) U' (R U R')"
('R', 'F', 'U'), ('R', 'F'): "R, U', R', F', U2, F"                     | "(R U' R') (F' U2 F)"
('U', 'R', 'F'), ('R', 'F'): "U2, R, U, R', F, R', F', R"               | "U2 (R U R') (F R' F' R)"
##########################################################
('F', 'R', 'D'), ('U', 'R'): "U', R', F, R, F', R, U, R'"               | "U' (R' F R F') (R U R')"
('R', 'D', 'F'), ('U', 'R'): "R, U', R', U, R, U', R'"                  | "(R U' R') U (R U' R')"
('D', 'F', 'R'), ('U', 'R'): "R, U, R', U', R, U, R'"                   | "(R U R') U' (R U R')"

('F', 'R', 'D'), ('F', 'U'): "U, R, U', R', F, R', F', R"               | "U (R U' R') (F R' F' R)"
('R', 'D', 'F'), ('F', 'U'): "F', U', F, U, F', U', F"                  | "y (L' U' L) U (L' U' L)"
('D', 'F', 'R'), ('F', 'U'): "F', U, F, U', F', U, F"                   | "y (L' U L) U' (L' U L)"
##########################################################
('F', 'R', 'D'), ('R', 'F'): "R2', U2, F, R2, F', U2, R', U, R'"        | "R2' U2' F R2 F' U2' R' U R'"

('R', 'D', 'F'), ('F', 'R'): "R, U', R', U', R, U, R', U2, R, U', R'"   | "(R U' R') U' (R U R' U2 R U' R')"
('R', 'D', 'F'), ('R', 'F'): "F', L', U2, L, F, R, U, R'"               | "(F' L' U2 L F) (R U R')"

('D', 'F', 'R'), ('F', 'R'): "R, U', R', U, R, U2, R', U, R, U', R'"    | "(R U' R') U (R U2 R' U R U' R')"
('D', 'F', 'R'), ('R', 'F'): "R, U', R', F', L', U2, L, F"              | "(R U' R') (F' L' U2 L F)"
##########################################################
('R', 'F', 'U'), ('F', 'L'): "U, F, U', F2, U, F"                       | "y U L U' L2' U L"
('F', 'U', 'R'), ('F', 'L'): "U', L', U', L, R, U', R'"                 | "U' (L' U' L) (R U' R')"
('U', 'R', 'F'), ('F', 'L'): "F, U, F2, U', F"                          | "y L U L2' U' L"

('R', 'F', 'U'), ('L', 'F'): "L, F', L2, U, L, U2, F"                   | "L F' L2' U L U2' F"
('U', 'R', 'F'), ('L', 'F'): "L', F', U', F, L"                         | "L' (F' U' F) L"
('F', 'U', 'R'), ('L', 'F'): "F, U2, F', R, U, R'"                      | "(F U2 F') (R U R')"

('R', 'F', 'U'), ('B', 'R'): "U', R', U, R2, U', R'"                    | "U' R' U R2 U' R'"
('F', 'U', 'R'), ('B', 'R'): "R', U', R2, U, R'"                        | "R' U' R2 U R'"
('U', 'R', 'F'), ('B', 'R'): "U, R, U, R', L', U, L"                    | "U (R U R') (L' U L)"

('R', 'F', 'U'), ('R', 'B'): "B', R, B2, U', B', U2, R'"                | "y R' F R2 U' R' U2 F'"
('F', 'U', 'R'), ('R', 'B'): "F, D, R, D', F'"                          | "F D R D' F'"
('U', 'R', 'F'), ('R', 'B'): "R', U2, R, F', U', F"                     | "R' U2 R F' U' F"

('L', 'B', 'U'), ('B', 'L'): "U2, R2, D, B2, D', R2"                    | y2 "U2 L2' u L2 u' L2'"
('B', 'U', 'L'), ('B', 'L'): "U, L, U, L', R, U, R'"                    | y2 "U (R U R') (L U L')"
('U', 'L', 'B'), ('B', 'L'): "U', L, U', L', R, U', R'"                 | y2 "U' (R U' R') (L U' L')"

('L', 'B', 'U'), ('L', 'B'): "R, B', U, B, R'"                          | y2 "L F' U F L'"
('B', 'U', 'L'), ('L', 'B'): "U2, B', R, U, R', B"                      | y2 "y2 f' U L U' f"
('U', 'L', 'B'), ('L', 'B'): "U2, L, F', U', F, L'"                     | y2 "U2' R (B' U' B) R'"
##########################################################
('R', 'B', 'D'), ('R', 'U'): ""                                         | y "U (R U' R') (L' U L)"
('B', 'D', 'R'), ('R', 'U'): ""                                         | y "U2 (R U' R') U (L' U' L)"
('D', 'R', 'B'), ('R', 'U'): ""                                         | y "(R U R') U' (L' U L)"

('R', 'B', 'D'), ('U', 'R'): ""                                         | y "y (L' U2 L) U' (L U L')"
('B', 'D', 'R'), ('U', 'R'): ""                                         | y "y U' L' U' L2 U2 L'"
('D', 'R', 'B'), ('U', 'R'): ""                                         | y "U' (R U R') (F U F')"
##########################################################
('L', 'F', 'D'), ('U', 'F'): ""                                         | y' "y U' (L' U L) (R U' R')"
('D', 'L', 'F'), ('U', 'F'): ""                                         | y' "y U2 (L' U L) U' (R U R')"
('F', 'D', 'L'), ('U', 'F'): ""                                         | y' "y (L' U' L) U (R U' R')"

('L', 'F', 'D'), ('F', 'U'): ""                                         | y' "(R U2 R') U (R' U' R)"
('D', 'L', 'F'), ('F', 'U'): ""                                         | y' "U R U R2' U2 R"
('F', 'D', 'L'), ('F', 'U'): ""                                         | y' "y U (L' U' L) (F' U' F)"
##########################################################
('B', 'L', 'D'), ('U', 'L'): ""                                         | y2 "U' (F' U F) (L U2 L')"
('L', 'D', 'B'), ('U', 'L'): ""                                         | y2 "(R U' R') (L U2 L')"
('D', 'B', 'L'), ('U', 'L'): ""                                         | y2 "(R U' R') U (L U L')"

('B', 'L', 'D'), ('B', 'U'): ""                                         | y2 "y U (F U' F') (R' U2 R)"
('L', 'D', 'B'), ('B', 'U'): ""                                         | y2 "y (L' U L) (R' U2 R)"
('D', 'B', 'L'), ('B', 'U'): ""                                         | y2 "(R' F R F') (L U2 L')"
##########################################################
('F', 'R', 'D'), ('B', 'R'): ""                                         | "R2 U' R2' U R2"
('F', 'R', 'D'), ('R', 'B'): ""                                         | "f' R' U R f"
('F', 'R', 'D'), ('F', 'L'): ""                                         | "F L F2' L' F'"
('F', 'R', 'D'), ('L', 'F'): ""                                         | "L u L' u' L'"
('F', 'R', 'D'), ('B', 'L'): ""                                         | y' "y L2' u L2 u' L2'"
('F', 'R', 'D'), ('L', 'B'): ""                                         | y' "y (L u L') U' (L u' L')"
##########################################################
('L', 'F', 'D'), ('L', 'F'): ""                                         | "R' F R2 U' R2' F' R"
('R', 'B', 'D'), ('R', 'B'): ""                                         | "y L F' L2' U L2 F L'"
('B', 'L', 'D'), ('B', 'L'): ""                                         | y2 "R (L U2 L') R'"
##########################################################
('D', 'L', 'F'), ('R', 'F'): ""                                         | "L F2' L' F U' F"
('F', 'D', 'L'), ('R', 'F'): ""                                         | "(R' F R U' F') (R U' R')"
('L', 'F', 'D'), ('R', 'F'): ""                                         | "y (L U F' L' F) U2 (L' U' L)"
('B', 'D', 'R'), ('R', 'F'): ""                                         | "y R' F2 R F' U F'"
('D', 'R', 'B'), ('R', 'F'): ""                                         | "y (L F' L' U F) (L' U L)"
('R', 'B', 'D'), ('R', 'F'): ""                                         | "(R' U' F R F') U2 (R U R')"
##########################################################
('R', 'D', 'F'), ('L', 'F'): ""                                         | "R U' R2' u' R' u R"
('D', 'F', 'R'), ('R', 'B'): ""                                         | "y (L F' L' U F) U' (L' U L)"
'''
white_cross(CUBE)
#printcube(CUBE)
print(f'  original F_MOVES: {F_MOVES}')
print(f'simplified F_MOVES: {simplify_F_MOVES(F_MOVES)}')
