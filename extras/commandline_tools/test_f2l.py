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
                            ['N', 'N', 'N']]), 
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


corner = eval(input('corner: '))
edge = eval(input('edge: '))
ALG = input('algorithm: ')
start = time.perf_counter()
ret = test_case_and_algorithm((corner, edge), ALG)
if ret==True:
    print("FAX!")
if ret==False:
    print("Nahhh thats CAP!")
print(time.perf_counter()-start)
if ret == True:
    with open('f2l_algs.txt', 'a') as file:
        file.write(f"{(corner, edge)}: " + '"' + ALG + '", \n')
