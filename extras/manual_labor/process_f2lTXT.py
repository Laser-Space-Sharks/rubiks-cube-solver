import numpy as np

indices_to_corners = [('L', 'B', 'U'), ('B', 'U', 'L'), ('U', 'L', 'B'),
                      ('B', 'R', 'U'), ('R', 'U', 'B'), ('U', 'B', 'R'),
                      ('R', 'F', 'U'), ('F', 'U', 'R'), ('U', 'R', 'F'),
                      ('F', 'L', 'U'), ('L', 'U', 'F'), ('U', 'F', 'L'),
                      ('B', 'L', 'D'), ('L', 'D', 'B'), ('D', 'B', 'L'),
                      ('R', 'B', 'D'), ('B', 'D', 'R'), ('D', 'R', 'B'),
                      ('F', 'R', 'D'), ('R', 'D', 'F'), ('D', 'F', 'R'),
                      ('L', 'F', 'D'), ('F', 'D', 'L'), ('D', 'L', 'F')]
corners_to_indices = {('L', 'B', 'U'): 0,
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

indices_to_edges = [('U', 'B'), ('B', 'U'), 
                    ('U', 'R'), ('R', 'U'), 
                    ('U', 'F'), ('F', 'U'),
                    ('U', 'L'), ('L', 'U'), 
                    ('B', 'L'), ('L', 'B'),
                    ('B', 'R'), ('R', 'B'),
                    ('F', 'R'), ('R', 'F'),
                    ('F', 'L'), ('L', 'F')]
edges_to_indices = {('U', 'B'): 0,
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

F2L_ALGORITHMS = [['' for _ in range(16)] for _ in range(24)]


import numpy as np
import time

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

def apply_alg_to_cube(cube, algorithm) -> None:
    cube2 = {}
    for key, value in cube.items():
        cube2[key] = np.copy(value)
    
    for move in algorithm.split():
        prime: bool = (move[-1]=="'")
        double: bool = (move[-1]=="2")
        mv = move[0]
        update_cube_with_F_move(cube2, mv, prime)
        if double:
            update_cube_with_F_move(cube2, mv, prime)
    
    return cube2

def find_extra_criteria(case):
    corner, edge = case
    # the white cross
    criteria = [('D', 0, 1, 'D'), ('D', 1, 0, 'D'), ('D', 1, 2, 'D'), ('D', 2, 1, 'D'), 
                ('B', 2, 1, 'B'), ('R', 2, 1, 'R'), ('F', 2, 1, 'F'), ('L', 2, 1, 'L')]
    preserved_slots = {('B', 'R'), ('B', 'L'), ('F', 'R'), ('F', 'L')}
    preserved_slots.difference_update(sorted(('F', 'R')))
    if 'D' in corner: preserved_slots.difference_update(sorted(tuple(set(corner).difference('D'))))
    if 'U' not in edge: preserved_slots.difference_update(sorted(tuple(set(edge))))
    
    if {'B', 'R'} in preserved_slots: criteria.extend([('R', 1, 2, 'R'), ('R', 2, 2, 'R'), ('B', 1, 0, 'B'), ('B', 2, 0, 'B'), ('D', 0, 0, 'D')])
    if {'B', 'L'} in preserved_slots: criteria.extend([('L', 1, 0, 'L'), ('L', 2, 0, 'L'), ('B', 1, 2, 'B'), ('B', 2, 2, 'B'), ('D', 0, 2, 'D')])
    if {'F', 'L'} in preserved_slots: criteria.extend([('L', 1, 2, 'L'), ('L', 2, 2, 'L'), ('F', 1, 0, 'F'), ('F', 2, 0, 'F'), ('D', 2, 2, 'D')])
    return criteria

def setup_case(case, cube):
    corner, edge = case
    if set(corner) == {'L', 'B', 'U'}: coords1 = {'L': (0, 0), 'B': (0, 2), 'U': (0, 0)}
    if set(corner) == {'B', 'R', 'U'}: coords1 = {'B': (0, 0), 'R': (0, 2), 'U': (0, 2)}
    if set(corner) == {'R', 'F', 'U'}: coords1 = {'R': (0, 0), 'F': (0, 2), 'U': (2, 2)}
    if set(corner) == {'F', 'L', 'U'}: coords1 = {'F': (0, 0), 'L': (0, 2), 'U': (2, 0)}
    if set(corner) == {'B', 'L', 'D'}: coords1 = {'B': (2, 2), 'L': (2, 0), 'D': (0, 2)}
    if set(corner) == {'R', 'B', 'D'}: coords1 = {'R': (2, 2), 'B': (2, 0), 'D': (0, 0)}
    if set(corner) == {'F', 'R', 'D'}: coords1 = {'F': (2, 2), 'R': (2, 0), 'D': (2, 0)}
    if set(corner) == {'L', 'F', 'D'}: coords1 = {'L': (2, 2), 'F': (2, 0), 'D': (2, 2)}

    if set(edge) == {'B', 'U'}: coords2 = {'B': (0, 1), 'U': (0, 1)}
    if set(edge) == {'R', 'U'}: coords2 = {'R': (0, 1), 'U': (1, 2)}
    if set(edge) == {'F', 'U'}: coords2 = {'F': (0, 1), 'U': (2, 1)}
    if set(edge) == {'L', 'U'}: coords2 = {'L': (0, 1), 'U': (1, 0)}
    if set(edge) == {'L', 'B'}: coords2 = {'L': (1, 0), 'B': (1, 2)}
    if set(edge) == {'B', 'R'}: coords2 = {'B': (1, 0), 'R': (1, 2)}
    if set(edge) == {'R', 'F'}: coords2 = {'R': (1, 0), 'F': (1, 2)}
    if set(edge) == {'F', 'L'}: coords2 = {'F': (1, 0), 'L': (1, 2)}

    for v1, v2 in zip(corner, ('F', 'R', 'D')): cube[v1][coords1[v1]] = v2
    for v1, v2 in zip(edge, ('F', 'R')): cube[v1][coords2[v1]] = v2

def test_case_and_algorithm(case, algorithm):
    cube = {'F': np.array([['N', 'N', 'N'], 
                            ['N', 'F', 'N'], 
                            ['N', 'N', 'N']]),
            'U': np.array([['N', 'N', 'N'], 
                            ['N', 'U', 'N'], 
                            ['N', 'F', 'N']]), 
            'R': np.array([['N', 'N', 'N'], 
                            ['N', 'R', 'N'], 
                            ['N', 'N', 'N']]),
            'D': np.array([['N', 'N', 'N'], 
                            ['N', 'D', 'N'], 
                            ['N', 'N', 'N']]), 
            'L': np.array([['N', 'N', 'N'], 
                            ['N', 'L', 'N'], 
                            ['N', 'N', 'N']]),  
            'B': np.array([['N', 'N', 'N'], 
                            ['N', 'B', 'N'], 
                            ['N', 'N', 'N']])}
    setup_case(case, cube)

    result_criteria = [('R', 1, 0, 'R'), ('R', 2, 0, 'R'), ('F', 1, 2, 'F'), ('F', 2, 2, 'F'), ('D', 2, 0, 'D')]
    for a1, a2, a3, a4 in find_extra_criteria(case):
        cube[a1][a2, a3] = a4
        result_criteria.append((a1, a2, a3, a4))
    
    resulting_cube = apply_alg_to_cube(cube, algorithm)
    for a1, a2, a3, a4 in result_criteria:
        if resulting_cube[a1][a2, a3] != a4: return False
    return True



with open("f2l_algs.txt", "r") as file:
    for line in file.readlines():
        line2 = line.strip().strip(",")
        part1, part2 = line2.split(": ")
        CASE, ALG = eval(part1), eval(part2)
        corner, edge = CASE
        if not test_case_and_algorithm((corner, edge), ALG): 
            print("AAAAAAAAAAAAAAAAAAAAAA")
            print(f"entry: {CASE} -> {ALG}")
        if F2L_ALGORITHMS[corners_to_indices[corner]][edges_to_indices[edge]] == '' or len(F2L_ALGORITHMS[corners_to_indices[corner]][edges_to_indices[edge]].split()) > len(ALG.split()):
            F2L_ALGORITHMS[corners_to_indices[corner]][edges_to_indices[edge]] = ALG

for i in F2L_ALGORITHMS:
    print(i)

print("\n----------------------------------------------------------------------\n")
for i in range(24):
    for j in range(16):
        if F2L_ALGORITHMS[i][j] == '':
            print((indices_to_corners[i], indices_to_edges[j]))
