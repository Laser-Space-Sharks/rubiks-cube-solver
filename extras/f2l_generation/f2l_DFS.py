import numpy as np
import time

# Using this program was also manual labor
def depth_first_search_algs2(cube3d, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far):
    if len(algorithm_so_far) == max_moves:
        go = False
        for a1, a2, a3, a4 in criteria:
            if cube3d[a1+2, a2+2, a3+2] != a4:
                go = True
                break
        if go == False:
            DISCOVERED_ALGS.append(algorithm_so_far)
        return
    opposites = [5, 3, 4, 1, 2, 0]
    move_types = [1, 1, 1, 1, 1, 1]
    if len(algorithm_so_far) >= 1: move_types[algorithm_so_far[-1][0]] = 0
    if len(algorithm_so_far) >= 2 and opposites[algorithm_so_far[-1][0]] == algorithm_so_far[-2][0]: move_types[algorithm_so_far[-2][0]] = 0
    if move_types[0]:
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,3:,:] = np.rot90(cube3d, k=b, axes=(2, 0))[:,3:,:]
            depth_first_search_algs2(cube3dnew, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far + [(0, b)])
    if move_types[1]:
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[3:,:,:] = np.rot90(cube3d, k=b, axes=(1, 2))[3:,:,:]
            depth_first_search_algs2(cube3dnew, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far + [(1, b)])
    if move_types[2]:
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,:,:2] = np.rot90(cube3d, k=b, axes=(1, 0))[:,:,:2]
            depth_first_search_algs2(cube3dnew, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far + [(2, b)])
    if move_types[3]:
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:2,:,:] = np.rot90(cube3d, k=b, axes=(2, 1))[:2,:,:]
            depth_first_search_algs2(cube3dnew, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far + [(3, b)])
    if move_types[4]:
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,:,3:] = np.rot90(cube3d, k=b, axes=(0, 1))[:,:,3:]
            depth_first_search_algs2(cube3dnew, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far + [(4, b)])
    if move_types[5]:
        for b in (1, -1, 2):
            cube3dnew = np.copy(cube3d)
            cube3dnew[:,:2,:] = np.rot90(cube3d, k=b, axes=(0, 2))[:,:2,:]
            depth_first_search_algs2(cube3dnew, max_moves, criteria, DISCOVERED_ALGS, algorithm_so_far + [(5, b)])

def mine_case(case, max_moves):
    cube2ds = {'F': np.array([['N', 'N', 'N'], 
                            ['N', 'F', 'N'], 
                            ['N', 'F', 'N']]),
            'U': np.array([['N', 'N', 'N'], 
                            ['N', 'U', 'N'], 
                            ['N', 'N', 'N']]), 
            'R': np.array([['N', 'N', 'N'], 
                            ['N', 'R', 'N'], 
                            ['N', 'R', 'N']]),
            'D': np.array([['N', 'D', 'N'], 
                            ['D', 'D', 'D'], 
                            ['N', 'D', 'N']]), 
            'L': np.array([['N', 'N', 'N'], 
                            ['N', 'L', 'N'], 
                            ['N', 'L', 'N']]),  
            'B': np.array([['N', 'N', 'N'], 
                            ['N', 'B', 'N'], 
                            ['N', 'B', 'N']])}
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

    for v1, v2 in zip(corner, ('F', 'R', 'D')): cube2ds[v1][coords1[v1]] = v2
    for v1, v2 in zip(edge, ('F', 'R')): cube2ds[v1][coords2[v1]] = v2

    criteria = [(1, -2, 0, 5), (0, -2, -1, 5), (-1, -2, 0, 5), (0, -2, 1, 5), 
                (2, -1, 0, 1), (0, -1, -2, 2), (-2, -1, 0, 3), (0, -1, 2, 4),
                (2, 0, -1, 1), (2, -1, -1, 1), (1, 0, -2, 2), (1, -1, -2, 2), (1, -2, -1, 5)]
    if cube2ds['R'][2, 2] == "N" and cube2ds['R'][1, 2] == "N": 
        for a1, a2, a3, a4 in [('R', 1, 2, 'R'), ('B', 1, 0, 'B'),
                               ('R', 2, 2, 'R'), ('B', 2, 0, 'B'), 
                                        ('D', 0, 0, 'D')]:
            cube2ds[a1][a2, a3] = a4
        criteria.extend([(2, 0, 1, 1), (1, 0, 2, 4), 
                         (2, -1, 1, 1), (1, -1, 2, 4), 
                                (1, -2, 1, 5)])
    if cube2ds['L'][2, 0] == "N" and cube2ds['L'][1, 0] == "N": 
        for a1, a2, a3, a4 in [('B', 1, 2, 'B'), ('L', 1, 0, 'L'), 
                               ('B', 2, 2, 'B'), ('L', 2, 0, 'L'), 
                                        ('D', 0, 2, 'D')]:
            cube2ds[a1][a2, a3] = a4
        criteria.extend([(-1, 0, 2, 4), (-2, 0, 1, 3),
                         (-1, -1, 2, 4), (-2, -1, 1, 3), 
                                (-1, -2, 1, 5)])
    if cube2ds['L'][2, 2] == "N" and cube2ds['L'][1, 2] == "N": 
        for a1, a2, a3, a4 in [('L', 1, 2, 'L'), ('F', 1, 0, 'F'), 
                               ('L', 2, 2, 'L'), ('F', 2, 0, 'F'), 
                                        ('D', 2, 2, 'D')]:
            cube2ds[a1][a2, a3] = a4
        criteria.extend([(-2, 0, -1, 3), (-1, 0, -2, 2),
                         (-2, -1, -1, 3), (-1, -1, -2, 2), 
                                (-1, -2, -1, 5)])
    # x, y, z point right, up, and back, respectively.
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
    DISCOVERED_ALGS = []
    #start = time.perf_counter()
    #depth_first_search_algs(cube3d, max_moves, criteria, DISCOVERED_ALGS, [])
    depth_first_search_algs2(cube3d, max_moves, criteria, DISCOVERED_ALGS, [])
    mods = ("", "", "2", "'")
    formatted = {' '.join([f'{"URFLBD"[a]}{mods[b]}' for a, b in DISCOVERED_ALG]) for DISCOVERED_ALG in DISCOVERED_ALGS}
    #print(formatted)
    #print(min(formatted, key=lambda x: len(x.split())))
    #print(time.perf_counter()-start)
    return formatted

"""
corner = eval(input('corner: '))
edge = eval(input('edge: '))
max_length = eval(input('current number of moves: '))
mine_case((corner, edge), max_length)
print(count2)
# F' L' U2 L F
"""
F2L_ALGORITHMS = [[set() for _ in range(16)] for _ in range(24)]
for i, corner in enumerate([('L', 'B', 'U'), ('B', 'U', 'L'), ('U', 'L', 'B'),
                      ('B', 'R', 'U'), ('R', 'U', 'B'), ('U', 'B', 'R'),
                      ('R', 'F', 'U'), ('F', 'U', 'R'), ('U', 'R', 'F'),
                      ('F', 'L', 'U'), ('L', 'U', 'F'), ('U', 'F', 'L'),
                      ('B', 'L', 'D'), ('L', 'D', 'B'), ('D', 'B', 'L'),
                      ('R', 'B', 'D'), ('B', 'D', 'R'), ('D', 'R', 'B'),
                      ('F', 'R', 'D'), ('R', 'D', 'F'), ('D', 'F', 'R'),
                      ('L', 'F', 'D'), ('F', 'D', 'L'), ('D', 'L', 'F')]):
    for j, edge in enumerate([('U', 'B'), ('B', 'U'), 
                    ('U', 'R'), ('R', 'U'), 
                    ('U', 'F'), ('F', 'U'),
                    ('U', 'L'), ('L', 'U'), 
                    ('B', 'L'), ('L', 'B'),
                    ('B', 'R'), ('R', 'B'),
                    ('F', 'R'), ('R', 'F'),
                    ('F', 'L'), ('L', 'F')]):
        for max_length in range(3, 7):
            best_algs = mine_case((corner, edge), max_length)
            print(f'{(i, j)}, max_length={max_length}: {best_algs}')
            if best_algs: 
                F2L_ALGORITHMS[i][j].update(best_algs)
                print(f'{(i, j)}: {best_algs}')
                break
        print('--------------------------------------------------')
def algorithm_inverter(algorithm: str):
    alg = algorithm.split()
    inverse = []
    for move in alg[::-1]:
        mv = move[0]
        if move[-1] == "2":inverse.append(f'{mv}2')
        elif move[-1] == "'":inverse.append(f'{mv}')
        else: inverse.append(f"{mv}'")
    return ' '.join(inverse)
"""
(0, 0), max_length=3: set()
(0, 0), max_length=4: set()
(0, 0), max_length=6: set()
--------------------------------------------------
(0, 1), max_length=3: set()
(0, 1), max_length=4: set()
(0, 1), max_length=5: set()
(0, 1), max_length=6: set()
--------------------------------------------------
(0, 2), max_length=3: set()
(0, 2), max_length=4: set()
(0, 2), max_length=5: {"R B' R B R2"}
(0, 2): {"R B' R B R2"}
--------------------------------------------------
(0, 3), max_length=3: set()
(0, 3), max_length=4: set()
(0, 3), max_length=5: set()
(0, 3), max_length=6: set()
--------------------------------------------------
(0, 4), max_length=3: set()
(0, 4), max_length=4: set()
(0, 4), max_length=5: set()
(0, 4), max_length=6: set()
--------------------------------------------------
(0, 5), max_length=3: set()
(0, 5), max_length=4: set()
(0, 5), max_length=5: {"F' L F' L' F2"}
(0, 5): {"F' L F' L' F2"}
--------------------------------------------------
(0, 6), max_length=3: set()
(0, 6), max_length=4: set()
(0, 6), max_length=5: set()
(0, 6), max_length=6: set()
--------------------------------------------------
(0, 7), max_length=3: set()
(0, 7), max_length=4: set()
(0, 7), max_length=5: set()
(0, 7), max_length=6: set()
--------------------------------------------------
(0, 8), max_length=3: set()
(0, 8), max_length=4: set()
(0, 8), max_length=5: set()
(0, 8), max_length=6: {"U2 F2 D' L2 D F2", "U2 R2 D B2 D' R2"}
(0, 8): {"U2 F2 D' L2 D F2", "U2 R2 D B2 D' R2"}
--------------------------------------------------
(0, 9), max_length=3: set()
(0, 9), max_length=4: set()
(0, 9), max_length=5: {"R B' U B R'", "F' L U' L' F"}
(0, 9): {"R B' U B R'", "F' L U' L' F"}
--------------------------------------------------
(0, 10), max_length=3: set()
(0, 10), max_length=4: set()
(0, 10), max_length=5: {"U R2 F R2 F'"}
(0, 10): {"U R2 F R2 F'"}
--------------------------------------------------
(0, 11), max_length=3: set()
(0, 11), max_length=4: set()
(0, 11), max_length=5: set()
(0, 11), max_length=6: set()
--------------------------------------------------
(0, 12), max_length=3: set()
(0, 12), max_length=4: set()
(0, 12), max_length=5: set()
(0, 12), max_length=6: set()
--------------------------------------------------
(0, 13), max_length=3: set()
(0, 13), max_length=4: set()
(0, 13), max_length=5: set()
(0, 13), max_length=6: set()
--------------------------------------------------
(0, 14), max_length=3: set()
(0, 14), max_length=4: set()
(0, 14), max_length=5: {"U' F2 R' F2 R"}
(0, 14): {"U' F2 R' F2 R"}
--------------------------------------------------
(0, 15), max_length=3: set()
(0, 15), max_length=4: set()
(0, 15), max_length=5: set()
(0, 15), max_length=6: set()
--------------------------------------------------
(1, 0), max_length=3: set()
(1, 0), max_length=4: set()
(1, 0), max_length=5: set()
(1, 0), max_length=6: set()
--------------------------------------------------
(1, 1), max_length=3: {"F' U2 F"}
(1, 1): {"F' U2 F"}
--------------------------------------------------
(1, 2), max_length=3: set()
(1, 2), max_length=4: set()
(1, 2), max_length=5: set()
(1, 2), max_length=6: set()
--------------------------------------------------
(1, 3), max_length=3: set()
(1, 3), max_length=4: set()
(1, 3), max_length=5: set()
(1, 3), max_length=6: {"F' L' U' L U2 F"}
(1, 3): {"F' L' U' L U2 F"}
--------------------------------------------------
(1, 4), max_length=3: set()
(1, 4), max_length=4: {"U2 R U R'"}
(1, 4): {"U2 R U R'"}
--------------------------------------------------
(1, 5), max_length=3: set()
(1, 5), max_length=4: set()
(1, 5), max_length=5: set()
(1, 5), max_length=6: set()
--------------------------------------------------
(1, 6), max_length=3: set()
(1, 6), max_length=4: set()
(1, 6), max_length=5: set()
(1, 6), max_length=6: set()
--------------------------------------------------
(1, 7), max_length=3: set()
(1, 7), max_length=4: set()
(1, 7), max_length=5: set()
(1, 7), max_length=6: set()
--------------------------------------------------
(1, 8), max_length=3: set()
(1, 8), max_length=4: set()
(1, 8), max_length=5: set()
(1, 8), max_length=6: set()
--------------------------------------------------
(1, 9), max_length=3: set()
(1, 9), max_length=4: set()
(1, 9), max_length=5: set()
(1, 9), max_length=6: {"U2 B' R U R' B"}
(1, 9): {"U2 B' R U R' B"}
--------------------------------------------------
(1, 10), max_length=3: set()
(1, 10), max_length=4: set()
(1, 10), max_length=5: {"U2 B' R2 B R2"}
(1, 10): {"U2 B' R2 B R2"}
--------------------------------------------------
(1, 11), max_length=3: set()
(1, 11), max_length=4: set()
(1, 11), max_length=5: set()
(1, 11), max_length=6: {"U2 B R U R' B'", "F' U2 R' U R F", "U2 F D R D' F'"}
(1, 11): {"U2 B R U R' B'", "F' U2 R' U R F", "U2 F D R D' F'"}
--------------------------------------------------
(1, 12), max_length=3: set()
(1, 12), max_length=4: set()
(1, 12), max_length=5: set()
(1, 12), max_length=6: {"F' L' U2 L U2 F"}
(1, 12): {"F' L' U2 L U2 F"}
--------------------------------------------------
(1, 13), max_length=3: set()
(1, 13), max_length=4: set()
(1, 13), max_length=5: set()
(1, 13), max_length=6: set()
--------------------------------------------------
(1, 14), max_length=3: set()
(1, 14), max_length=4: set()
(1, 14), max_length=5: set()
(1, 14), max_length=6: set()
--------------------------------------------------
(1, 15), max_length=3: set()
(1, 15), max_length=4: set()
(1, 15), max_length=5: set()
(1, 15), max_length=6: {"L' U L F' U2 F"}
(1, 15): {"L' U L F' U2 F"}
--------------------------------------------------
(2, 0), max_length=3: set()
(2, 0), max_length=4: set()
(2, 0), max_length=5: set()
(2, 0), max_length=6: set()
--------------------------------------------------
(2, 1), max_length=3: set()
(2, 1), max_length=4: set()
(2, 1), max_length=5: set()
(2, 1), max_length=6: set()
--------------------------------------------------
(2, 2), max_length=3: set()
(2, 2), max_length=4: set()
(2, 2), max_length=5: set()
(2, 2), max_length=6: set()
--------------------------------------------------
(2, 3), max_length=3: set()
(2, 3), max_length=4: {"U2 F' U' F"}
(2, 3): {"U2 F' U' F"}
--------------------------------------------------
(2, 4), max_length=3: set()
(2, 4), max_length=4: set()
(2, 4), max_length=5: set()
(2, 4), max_length=6: {"R B U B' U2 R'"}
(2, 4): {"R B U B' U2 R'"}
--------------------------------------------------
(2, 5), max_length=3: set()
(2, 5), max_length=4: set()
(2, 5), max_length=5: set()
(2, 5), max_length=6: set()
--------------------------------------------------
(2, 6), max_length=3: {"R U2 R'"}
(2, 6): {"R U2 R'"}
--------------------------------------------------
(2, 7), max_length=3: set()
(2, 7), max_length=4: set()
(2, 7), max_length=5: set()
(2, 7), max_length=6: set()
--------------------------------------------------
(2, 8), max_length=3: set()
(2, 8), max_length=4: set()
(2, 8), max_length=5: set()
(2, 8), max_length=6: set()
--------------------------------------------------
(2, 9), max_length=3: set()
(2, 9), max_length=4: set()
(2, 9), max_length=5: set()
(2, 9), max_length=6: {"U2 L F' U' F L'"}
(2, 9): {"U2 L F' U' F L'"}
--------------------------------------------------
(2, 10), max_length=3: set()
(2, 10), max_length=4: set()
(2, 10), max_length=5: set()
(2, 10), max_length=6: set()
--------------------------------------------------
(2, 11), max_length=3: set()
(2, 11), max_length=4: set()
(2, 11), max_length=5: set()
(2, 11), max_length=6: {"B U' B' R U2 R'"}
(2, 11): {"B U' B' R U2 R'"}
--------------------------------------------------
(2, 12), max_length=3: set()
(2, 12), max_length=4: set()
(2, 12), max_length=5: set()
(2, 12), max_length=6: {"R B U2 B' U2 R'"}
(2, 12): {"R B U2 B' U2 R'"}
--------------------------------------------------
(2, 13), max_length=3: set()
(2, 13), max_length=4: set()
(2, 13), max_length=5: set()
(2, 13), max_length=6: set()
--------------------------------------------------
(2, 14), max_length=3: set()
(2, 14), max_length=4: set()
(2, 14), max_length=5: {"U2 L F2 L' F2"}
(2, 14): {"U2 L F2 L' F2"}
--------------------------------------------------
(2, 15), max_length=3: set()
(2, 15), max_length=4: set()
(2, 15), max_length=5: set()
(2, 15), max_length=6: {"U2 L' F' U' F L", "U2 R' D' F' D R", "R U2 F U' F' R'"}
(2, 15): {"U2 L' F' U' F L", "U2 R' D' F' D R", "R U2 F U' F' R'"}
--------------------------------------------------
(3, 0), max_length=3: set()
(3, 0), max_length=4: set()
(3, 0), max_length=5: set()
(3, 0), max_length=6: set()
--------------------------------------------------
(3, 1), max_length=3: set()
(3, 1), max_length=4: set()
(3, 1), max_length=5: set()
(3, 1), max_length=6: set()
--------------------------------------------------
(3, 2), max_length=3: set()
(3, 2), max_length=4: set()
(3, 2), max_length=5: set()
(3, 2), max_length=6: set()
--------------------------------------------------
(3, 3), max_length=3: set()
(3, 3), max_length=4: set()
(3, 3), max_length=5: set()
(3, 3), max_length=6: set()
--------------------------------------------------
(3, 4), max_length=3: set()
(3, 4), max_length=4: set()
(3, 4), max_length=5: set()
(3, 4), max_length=6: {"U R B U2 B' R'", "U' R B' R B R2"}
(3, 4): {"U R B U2 B' R'", "U' R B' R B R2"}
--------------------------------------------------
(3, 5), max_length=3: set()
(3, 5), max_length=4: set()
(3, 5), max_length=5: set()
(3, 5), max_length=6: {"F' U2 F2 R' F' R", "F' U' L F' L' F2"}
(3, 5): {"F' U2 F2 R' F' R", "F' U' L F' L' F2"}
--------------------------------------------------
(3, 6), max_length=3: set()
(3, 6), max_length=4: set()
(3, 6), max_length=5: set()
(3, 6), max_length=6: set()
--------------------------------------------------
(3, 7), max_length=3: set()
(3, 7), max_length=4: set()
(3, 7), max_length=5: set()
(3, 7), max_length=6: {"U' F' L F' L' F2", "U F' L' U2 L F", "F' L' U' L U' F"}
(3, 7): {"U' F' L F' L' F2", "U F' L' U2 L F", "F' L' U' L U' F"}
--------------------------------------------------
(3, 8), max_length=3: set()
(3, 8), max_length=4: set()
(3, 8), max_length=5: set()
(3, 8), max_length=6: {"U R2 D B2 D' R2", "U F2 D' L2 D F2"}
(3, 8): {"U R2 D B2 D' R2", "U F2 D' L2 D F2"}
--------------------------------------------------
(3, 9), max_length=3: set()
(3, 9), max_length=4: set()
(3, 9), max_length=5: set()
(3, 9), max_length=6: {"U' F' L U' L' F", "F' U' L U' L' F", "U' R B' U B R'"}
(3, 9): {"U' F' L U' L' F", "F' U' L U' L' F", "U' R B' U B R'"}
--------------------------------------------------
(3, 10), max_length=3: set()
(3, 10), max_length=4: {"R2 F R2 F'"}
(3, 10): {"R2 F R2 F'"}
--------------------------------------------------
(3, 11), max_length=3: set()
(3, 11), max_length=4: set()
(3, 11), max_length=5: set()
(3, 11), max_length=6: set()
--------------------------------------------------
(3, 12), max_length=3: set()
(3, 12), max_length=4: set()
(3, 12), max_length=5: set()
(3, 12), max_length=6: set()
--------------------------------------------------
(3, 13), max_length=3: set()
(3, 13), max_length=4: set()
(3, 13), max_length=5: set()
(3, 13), max_length=6: {"F' U2 F R U R'"}
(3, 13): {"F' U2 F R U R'"}
--------------------------------------------------
(3, 14), max_length=3: set()
(3, 14), max_length=4: set()
(3, 14), max_length=5: {"U2 F2 R' F2 R"}
(3, 14): {"U2 F2 R' F2 R"}
--------------------------------------------------
(3, 15), max_length=3: set()
(3, 15), max_length=4: set()
(3, 15), max_length=5: set()
(3, 15), max_length=6: set()
--------------------------------------------------
(4, 0), max_length=3: set()
(4, 0), max_length=4: set()
(4, 0), max_length=5: set()
(4, 0), max_length=6: set()
--------------------------------------------------
(4, 1), max_length=3: set()
(4, 1), max_length=4: set()
(4, 1), max_length=5: set()
(4, 1), max_length=6: set()
--------------------------------------------------
(4, 2), max_length=3: set()
(4, 2), max_length=4: set()
(4, 2), max_length=5: set()
(4, 2), max_length=6: set()
--------------------------------------------------
(4, 3), max_length=3: {"F' U F"}
(4, 3): {"F' U F"}
--------------------------------------------------
(4, 4), max_length=3: set()
(4, 4), max_length=4: set()
(4, 4), max_length=5: set()
(4, 4), max_length=6: set()
--------------------------------------------------
(4, 5), max_length=3: set()
(4, 5), max_length=4: set()
(4, 5), max_length=5: set()
(4, 5), max_length=6: set()
--------------------------------------------------
(4, 6), max_length=3: set()
(4, 6), max_length=4: {"U R U R'"}
(4, 6): {"U R U R'"}
--------------------------------------------------
(4, 7), max_length=3: set()
(4, 7), max_length=4: set()
(4, 7), max_length=5: set()
(4, 7), max_length=6: set()
--------------------------------------------------
(4, 8), max_length=3: set()
(4, 8), max_length=4: set()
(4, 8), max_length=5: set()
(4, 8), max_length=6: {"B' U F' B U F", "L U R L' U R'", "L U R U R' L'", "B' U B F' U F", "L U R U L' R'", "B' F' U B U F", "F' B' U B U F", "L U L' R U R'"}
(4, 8): {"B' U F' B U F", "L U R L' U R'", "L U R U R' L'", "B' U B F' U F", "L U R U L' R'", "B' F' U B U F", "F' B' U B U F", "L U L' R U R'"}
--------------------------------------------------
(4, 9), max_length=3: set()
(4, 9), max_length=4: set()
(4, 9), max_length=5: set()
(4, 9), max_length=6: {"U B' R U R' B"}
(4, 9): {"U B' R U R' B"}
--------------------------------------------------
(4, 10), max_length=3: set()
(4, 10), max_length=4: set()
(4, 10), max_length=5: {"U B' R2 B R2"}
(4, 10): {"U B' R2 B R2"}
--------------------------------------------------
(4, 11), max_length=3: set()
(4, 11), max_length=4: set()
(4, 11), max_length=5: set()
(4, 11), max_length=6: {"U F D R D' F'", "F' U R' U R F", "U B R U R' B'"}
(4, 11): {"U F D R D' F'", "F' U R' U R F", "U B R U R' B'"}
--------------------------------------------------
(4, 12), max_length=3: set()
(4, 12), max_length=4: set()
(4, 12), max_length=5: set()
(4, 12), max_length=6: set()
--------------------------------------------------
(4, 13), max_length=3: set()
(4, 13), max_length=4: set()
(4, 13), max_length=5: set()
(4, 13), max_length=6: set()
--------------------------------------------------
(4, 14), max_length=3: set()
(4, 14), max_length=4: set()
(4, 14), max_length=5: set()
(4, 14), max_length=6: {"L' U R L U R'", "L' U' L R U' R'", "L' U R U L R'", "L' U L R U R'", "L' U' R L U' R'", "L' U R U R' L"}
(4, 14): {"L' U R L U R'", "L' U' L R U' R'", "L' U R U L R'", "L' U L R U R'", "L' U' R L U' R'", "L' U R U R' L"}
--------------------------------------------------
(4, 15), max_length=3: set()
(4, 15), max_length=4: set()
(4, 15), max_length=5: set()
(4, 15), max_length=6: set()
--------------------------------------------------
(5, 0), max_length=3: set()
(5, 0), max_length=4: {"U2 R U' R'", "U' R U2 R'"}
(5, 0): {"U2 R U' R'", "U' R U2 R'"}
--------------------------------------------------
(5, 1), max_length=3: set()
(5, 1), max_length=4: set()
(5, 1), max_length=5: set()
(5, 1), max_length=6: set()
--------------------------------------------------
(5, 2), max_length=3: set()
(5, 2), max_length=4: set()
(5, 2), max_length=5: set()
(5, 2), max_length=6: set()
--------------------------------------------------
(5, 3), max_length=3: set()
(5, 3), max_length=4: set()
(5, 3), max_length=5: set()
(5, 3), max_length=6: set()
--------------------------------------------------
(5, 4), max_length=3: set()
(5, 4), max_length=4: set()
(5, 4), max_length=5: set()
(5, 4), max_length=6: set()
--------------------------------------------------
(5, 5), max_length=3: set()
(5, 5), max_length=4: {"U F' U' F"}
(5, 5): {"U F' U' F"}
--------------------------------------------------
(5, 6), max_length=3: set()
(5, 6), max_length=4: set()
(5, 6), max_length=5: set()
(5, 6), max_length=6: set()
--------------------------------------------------
(5, 7), max_length=3: set()
(5, 7), max_length=4: set()
(5, 7), max_length=5: set()
(5, 7), max_length=6: set()
--------------------------------------------------
(5, 8), max_length=3: set()
(5, 8), max_length=4: set()
(5, 8), max_length=5: set()
(5, 8), max_length=6: set()
--------------------------------------------------
(5, 9), max_length=3: set()
(5, 9), max_length=4: set()
(5, 9), max_length=5: set()
(5, 9), max_length=6: {"U L F' U' F L'"}
(5, 9): {"U L F' U' F L'"}
--------------------------------------------------
(5, 10), max_length=3: set()
(5, 10), max_length=4: set()
(5, 10), max_length=5: set()
(5, 10), max_length=6: set()
--------------------------------------------------
(5, 11), max_length=3: set()
(5, 11), max_length=4: set()
(5, 11), max_length=5: set()
(5, 11), max_length=6: set()
--------------------------------------------------
(5, 12), max_length=3: set()
(5, 12), max_length=4: set()
(5, 12), max_length=5: set()
(5, 12), max_length=6: {"F' U2 L' U2 L F"}
(5, 12): {"F' U2 L' U2 L F"}
--------------------------------------------------
(5, 13), max_length=3: set()
(5, 13), max_length=4: set()
(5, 13), max_length=5: set()
(5, 13), max_length=6: set()
--------------------------------------------------
(5, 14), max_length=3: set()
(5, 14), max_length=4: set()
(5, 14), max_length=5: {"U L F2 L' F2"}
(5, 14): {"U L F2 L' F2"}
--------------------------------------------------
(5, 15), max_length=3: set()
(5, 15), max_length=4: set()
(5, 15), max_length=5: set()
(5, 15), max_length=6: {"U R' D' F' D R", "U L' F' U' F L"}
(5, 15): {"U R' D' F' D R", "U L' F' U' F L"}
--------------------------------------------------
(6, 0), max_length=3: set()
(6, 0), max_length=4: set()
(6, 0), max_length=5: set()
(6, 0), max_length=6: set()
--------------------------------------------------
(6, 1), max_length=3: set()
(6, 1), max_length=4: set()
(6, 1), max_length=5: {"F' L' U2 L F"}
(6, 1): {"F' L' U2 L F"}
--------------------------------------------------
(6, 2), max_length=3: set()
(6, 2), max_length=4: set()
(6, 2), max_length=5: set()
(6, 2), max_length=6: set()
--------------------------------------------------
(6, 3), max_length=3: set()
(6, 3), max_length=4: set()
(6, 3), max_length=5: set()
(6, 3), max_length=6: set()
--------------------------------------------------
(6, 4), max_length=3: set()
(6, 4), max_length=4: set()
(6, 4), max_length=5: set()
(6, 4), max_length=6: set()
--------------------------------------------------
(6, 5), max_length=3: set()
(6, 5), max_length=4: set()
(6, 5), max_length=5: set()
(6, 5), max_length=6: set()
--------------------------------------------------
(6, 6), max_length=3: set()
(6, 6), max_length=4: set()
(6, 6), max_length=5: {"R B U2 B' R'"}
(6, 6): {"R B U2 B' R'"}
--------------------------------------------------
(6, 7), max_length=3: set()
(6, 7), max_length=4: set()
(6, 7), max_length=5: set()
(6, 7), max_length=6: set()
--------------------------------------------------
(6, 8), max_length=3: set()
(6, 8), max_length=4: set()
(6, 8), max_length=5: {"F2 D' L2 D F2", "R2 D B2 D' R2"}
(6, 8): {"F2 D' L2 D F2", "R2 D B2 D' R2"}
--------------------------------------------------
(6, 9), max_length=3: set()
(6, 9), max_length=4: set()
(6, 9), max_length=5: set()
(6, 9), max_length=6: {"U2 F' L U' L' F", "U2 R B' U B R'"}
(6, 9): {"U2 F' L U' L' F", "U2 R B' U B R'"}
--------------------------------------------------
(6, 10), max_length=3: set()
(6, 10), max_length=4: set()
(6, 10), max_length=5: {"R F R2 F' R'", "R2 U R2 U' R2", "U' R2 F R2 F'"}
(6, 10): {"R F R2 F' R'", "R2 U R2 U' R2", "U' R2 F R2 F'"}
--------------------------------------------------
(6, 11), max_length=3: set()
(6, 11), max_length=4: set()
(6, 11), max_length=5: set()
(6, 11), max_length=6: set()
--------------------------------------------------
(6, 12), max_length=3: set()
(6, 12), max_length=4: set()
(6, 12), max_length=5: set()
(6, 12), max_length=6: set()
--------------------------------------------------
(6, 13), max_length=3: set()
(6, 13), max_length=4: set()
(6, 13), max_length=5: set()
(6, 13), max_length=6: {"F' U F R U2 R'", "R U' R' F' U2 F"}
(6, 13): {"F' U F R U2 R'", "R U' R' F' U2 F"}
--------------------------------------------------
(6, 14), max_length=3: set()
(6, 14), max_length=4: set()
(6, 14), max_length=5: {"F' R' F2 R F", "U F2 R' F2 R", "F2 U' F2 U F2"}
(6, 14): {"F' R' F2 R F", "U F2 R' F2 R", "F2 U' F2 U F2"}
--------------------------------------------------
(6, 15), max_length=3: set()
(6, 15), max_length=4: set()
(6, 15), max_length=5: set()
(6, 15), max_length=6: set()
--------------------------------------------------
(7, 0), max_length=3: {"R U R'"}
(7, 0): {"R U R'"}
--------------------------------------------------
(7, 1), max_length=3: set()
(7, 1), max_length=4: set()
(7, 1), max_length=5: set()
(7, 1), max_length=6: set()
--------------------------------------------------
(7, 2), max_length=3: set()
(7, 2), max_length=4: set()
(7, 2), max_length=5: set()
(7, 2), max_length=6: set()
--------------------------------------------------
(7, 3), max_length=3: set()
(7, 3), max_length=4: set()
(7, 3), max_length=5: set()
(7, 3), max_length=6: set()
--------------------------------------------------
(7, 4), max_length=3: set()
(7, 4), max_length=4: set()
(7, 4), max_length=5: set()
(7, 4), max_length=6: set()
--------------------------------------------------
(7, 5), max_length=3: set()
(7, 5), max_length=4: {"F R' F' R", "U2 F' U2 F", "U' F' U F"}
(7, 5): {"F R' F' R", "U2 F' U2 F", "U' F' U F"}
--------------------------------------------------
(7, 6), max_length=3: set()
(7, 6), max_length=4: set()
(7, 6), max_length=5: set()
(7, 6), max_length=6: {"F2 U' L' U L F2"}
(7, 6): {"F2 U' L' U L F2"}
--------------------------------------------------
(7, 7), max_length=3: set()
(7, 7), max_length=4: set()
(7, 7), max_length=5: set()
(7, 7), max_length=6: {"R2 B U B' U' R2"}
(7, 7): {"R2 B U B' U' R2"}
--------------------------------------------------
(7, 8), max_length=3: set()
(7, 8), max_length=4: set()
(7, 8), max_length=5: set()
(7, 8), max_length=6: set()
--------------------------------------------------
(7, 9), max_length=3: set()
(7, 9), max_length=4: set()
(7, 9), max_length=5: {"B' R U R' B"}
(7, 9): {"B' R U R' B"}
--------------------------------------------------
(7, 10), max_length=3: set()
(7, 10), max_length=4: {"B' R2 B R2"}
(7, 10): {"B' R2 B R2"}
--------------------------------------------------
(7, 11), max_length=3: set()
(7, 11), max_length=4: set()
(7, 11), max_length=5: {"F D R D' F'", "B R U R' B'"}
(7, 11): {"F D R D' F'", "B R U R' B'"}
--------------------------------------------------
(7, 12), max_length=3: set()
(7, 12), max_length=4: set()
(7, 12), max_length=5: set()
(7, 12), max_length=6: set()
--------------------------------------------------
(7, 13), max_length=3: set()
(7, 13), max_length=4: set()
(7, 13), max_length=5: set()
(7, 13), max_length=6: set()
--------------------------------------------------
(7, 14), max_length=3: set()
(7, 14), max_length=4: set()
(7, 14), max_length=5: set()
(7, 14), max_length=6: set()
--------------------------------------------------
(7, 15), max_length=3: set()
(7, 15), max_length=4: set()
(7, 15), max_length=5: set()
(7, 15), max_length=6: {"R U F U' F' R'", "F U2 F' R U R'"}
(7, 15): {"R U F U' F' R'", "F U2 F' R U R'"}
--------------------------------------------------
(8, 0), max_length=3: set()
(8, 0), max_length=4: set()
(8, 0), max_length=5: set()
(8, 0), max_length=6: {"F2 L' U' L U F2"}
(8, 0): {"F2 L' U' L U F2"}
--------------------------------------------------
(8, 1), max_length=3: set()
(8, 1), max_length=4: set()
(8, 1), max_length=5: set()
(8, 1), max_length=6: {"R2 U B U' B' R2"}
(8, 1): {"R2 U B U' B' R2"}
--------------------------------------------------
(8, 2), max_length=3: set()
(8, 2), max_length=4: {"U R U' R'", "U2 R U2 R'", "R' F R F'"}
(8, 2): {"U R U' R'", "U2 R U2 R'", "R' F R F'"}
--------------------------------------------------
(8, 3), max_length=3: set()
(8, 3), max_length=4: set()
(8, 3), max_length=5: set()
(8, 3), max_length=6: set()
--------------------------------------------------
(8, 4), max_length=3: set()
(8, 4), max_length=4: set()
(8, 4), max_length=5: set()
(8, 4), max_length=6: set()
--------------------------------------------------
(8, 5), max_length=3: set()
(8, 5), max_length=4: set()
(8, 5), max_length=5: set()
(8, 5), max_length=6: set()
--------------------------------------------------
(8, 6), max_length=3: set()
(8, 6), max_length=4: set()
(8, 6), max_length=5: set()
(8, 6), max_length=6: set()
--------------------------------------------------
(8, 7), max_length=3: {"F' U' F"}
(8, 7): {"F' U' F"}
--------------------------------------------------
(8, 8), max_length=3: set()
(8, 8), max_length=4: set()
(8, 8), max_length=5: set()
(8, 8), max_length=6: set()
--------------------------------------------------
(8, 9), max_length=3: set()
(8, 9), max_length=4: set()
(8, 9), max_length=5: {"L F' U' F L'"}
(8, 9): {"L F' U' F L'"}
--------------------------------------------------
(8, 10), max_length=3: set()
(8, 10), max_length=4: set()
(8, 10), max_length=5: set()
(8, 10), max_length=6: set()
--------------------------------------------------
(8, 11), max_length=3: set()
(8, 11), max_length=4: set()
(8, 11), max_length=5: set()
(8, 11), max_length=6: {"F' U' R' U R F", "R' U2 R F' U' F"}
(8, 11): {"F' U' R' U R F", "R' U2 R F' U' F"}
--------------------------------------------------
(8, 12), max_length=3: set()
(8, 12), max_length=4: set()
(8, 12), max_length=5: set()
(8, 12), max_length=6: set()
--------------------------------------------------
(8, 13), max_length=3: set()
(8, 13), max_length=4: set()
(8, 13), max_length=5: set()
(8, 13), max_length=6: set()
--------------------------------------------------
(8, 14), max_length=3: set()
(8, 14), max_length=4: {"L F2 L' F2"}
(8, 14): {"L F2 L' F2"}
--------------------------------------------------
(8, 15), max_length=3: set()
(8, 15), max_length=4: set()
(8, 15), max_length=5: {"L' F' U' F L", "R' D' F' D R"}
(8, 15): {"L' F' U' F L", "R' D' F' D R"}
--------------------------------------------------
(9, 0), max_length=3: set()
(9, 0), max_length=4: set()
(9, 0), max_length=5: set()
(9, 0), max_length=6: {"R B U B' U R'", "U R B' R B R2", "U' R B U2 B' R'"}
(9, 0): {"R B U B' U R'", "U R B' R B R2", "U' R B U2 B' R'"}
--------------------------------------------------
(9, 1), max_length=3: set()
(9, 1), max_length=4: set()
(9, 1), max_length=5: set()
(9, 1), max_length=6: set()
--------------------------------------------------
(9, 2), max_length=3: set()
(9, 2), max_length=4: set()
(9, 2), max_length=5: set()
(9, 2), max_length=6: {"R U B' R B R2", "R U2 R2 F R F'"}
(9, 2): {"R U B' R B R2", "R U2 R2 F R F'"}
--------------------------------------------------
(9, 3), max_length=3: set()
(9, 3), max_length=4: set()
(9, 3), max_length=5: set()
(9, 3), max_length=6: {"U' F' L' U2 L F", "U F' L F' L' F2"}
(9, 3): {"U' F' L' U2 L F", "U F' L F' L' F2"}
--------------------------------------------------
(9, 4), max_length=3: set()
(9, 4), max_length=4: set()
(9, 4), max_length=5: set()
(9, 4), max_length=6: set()
--------------------------------------------------
(9, 5), max_length=3: set()
(9, 5), max_length=4: set()
(9, 5), max_length=5: set()
(9, 5), max_length=6: set()
--------------------------------------------------
(9, 6), max_length=3: set()
(9, 6), max_length=4: set()
(9, 6), max_length=5: set()
(9, 6), max_length=6: set()
--------------------------------------------------
(9, 7), max_length=3: set()
(9, 7), max_length=4: set()
(9, 7), max_length=5: set()
(9, 7), max_length=6: set()
--------------------------------------------------
(9, 8), max_length=3: set()
(9, 8), max_length=4: set()
(9, 8), max_length=5: set()
(9, 8), max_length=6: {"U' R2 D B2 D' R2", "U' F2 D' L2 D F2"}
(9, 8): {"U' R2 D B2 D' R2", "U' F2 D' L2 D F2"}
--------------------------------------------------
(9, 9), max_length=3: set()
(9, 9), max_length=4: set()
(9, 9), max_length=5: set()
(9, 9), max_length=6: {"R U B' U B R'", "U R B' U B R'", "U F' L U' L' F"}
(9, 9): {"R U B' U B R'", "U R B' U B R'", "U F' L U' L' F"}
--------------------------------------------------
(9, 10), max_length=3: set()
(9, 10), max_length=4: set()
(9, 10), max_length=5: {"U2 R2 F R2 F'"}
(9, 10): {"U2 R2 F R2 F'"}
--------------------------------------------------
(9, 11), max_length=3: set()
(9, 11), max_length=4: set()
(9, 11), max_length=5: set()
(9, 11), max_length=6: set()
--------------------------------------------------
(9, 12), max_length=3: set()
(9, 12), max_length=4: set()
(9, 12), max_length=5: set()
(9, 12), max_length=6: set()
--------------------------------------------------
(9, 13), max_length=3: set()
(9, 13), max_length=4: set()
(9, 13), max_length=5: set()
(9, 13), max_length=6: {"R U2 R' F' U' F"}
(9, 13): {"R U2 R' F' U' F"}
--------------------------------------------------
(9, 14), max_length=3: set()
(9, 14), max_length=4: {"F2 R' F2 R"}
(9, 14): {"F2 R' F2 R"}
--------------------------------------------------
(9, 15), max_length=3: set()
(9, 15), max_length=4: set()
(9, 15), max_length=5: set()
(9, 15), max_length=6: set()
--------------------------------------------------
(10, 0), max_length=3: set()
(10, 0), max_length=4: set()
(10, 0), max_length=5: set()
(10, 0), max_length=6: set()
--------------------------------------------------
(10, 1), max_length=3: set()
(10, 1), max_length=4: set()
(10, 1), max_length=5: set()
(10, 1), max_length=6: set()
--------------------------------------------------
(10, 2), max_length=3: set()
(10, 2), max_length=4: {"U' R U R'"}
(10, 2): {"U' R U R'"}
--------------------------------------------------
(10, 3), max_length=3: set()
(10, 3), max_length=4: set()
(10, 3), max_length=5: set()
(10, 3), max_length=6: set()
--------------------------------------------------
(10, 4), max_length=3: set()
(10, 4), max_length=4: set()
(10, 4), max_length=5: set()
(10, 4), max_length=6: set()
--------------------------------------------------
(10, 5), max_length=3: set()
(10, 5), max_length=4: set()
(10, 5), max_length=5: set()
(10, 5), max_length=6: set()
--------------------------------------------------
(10, 6), max_length=3: set()
(10, 6), max_length=4: set()
(10, 6), max_length=5: set()
(10, 6), max_length=6: set()
--------------------------------------------------
(10, 7), max_length=3: set()
(10, 7), max_length=4: {"U2 F' U F", "U F' U2 F"}
(10, 7): {"U2 F' U F", "U F' U2 F"}
--------------------------------------------------
(10, 8), max_length=3: set()
(10, 8), max_length=4: set()
(10, 8), max_length=5: set()
(10, 8), max_length=6: set()
--------------------------------------------------
(10, 9), max_length=3: set()
(10, 9), max_length=4: set()
(10, 9), max_length=5: set()
(10, 9), max_length=6: {"U' B' R U R' B"}
(10, 9): {"U' B' R U R' B"}
--------------------------------------------------
(10, 10), max_length=3: set()
(10, 10), max_length=4: set()
(10, 10), max_length=5: {"U' B' R2 B R2"}
(10, 10): {"U' B' R2 B R2"}
--------------------------------------------------
(10, 11), max_length=3: set()
(10, 11), max_length=4: set()
(10, 11), max_length=5: set()
(10, 11), max_length=6: {"U' B R U R' B'", "U' F D R D' F'"}
(10, 11): {"U' B R U R' B'", "U' F D R D' F'"}
--------------------------------------------------
(10, 12), max_length=3: set()
(10, 12), max_length=4: set()
(10, 12), max_length=5: set()
(10, 12), max_length=6: {"R U2 B U2 B' R'"}
(10, 12): {"R U2 B U2 B' R'"}
--------------------------------------------------
(10, 13), max_length=3: set()
(10, 13), max_length=4: set()
(10, 13), max_length=5: set()
(10, 13), max_length=6: set()
--------------------------------------------------
(10, 14), max_length=3: set()
(10, 14), max_length=4: set()
(10, 14), max_length=5: set()
(10, 14), max_length=6: set()
--------------------------------------------------
(10, 15), max_length=3: set()
(10, 15), max_length=4: set()
(10, 15), max_length=5: set()
(10, 15), max_length=6: set()
--------------------------------------------------
(11, 0), max_length=3: set()
(11, 0), max_length=4: set()
(11, 0), max_length=5: set()
(11, 0), max_length=6: set()
--------------------------------------------------
(11, 1), max_length=3: set()
(11, 1), max_length=4: {"U' F' U' F"}
(11, 1): {"U' F' U' F"}
--------------------------------------------------
(11, 2), max_length=3: set()
(11, 2), max_length=4: set()
(11, 2), max_length=5: set()
(11, 2), max_length=6: set()
--------------------------------------------------
(11, 3), max_length=3: set()
(11, 3), max_length=4: set()
(11, 3), max_length=5: set()
(11, 3), max_length=6: set()
--------------------------------------------------
(11, 4), max_length=3: {"R U' R'"}
(11, 4): {"R U' R'"}
--------------------------------------------------
(11, 5), max_length=3: set()
(11, 5), max_length=4: set()
(11, 5), max_length=5: set()
(11, 5), max_length=6: set()
--------------------------------------------------
(11, 6), max_length=3: set()
(11, 6), max_length=4: set()
(11, 6), max_length=5: set()
(11, 6), max_length=6: set()
--------------------------------------------------
(11, 7), max_length=3: set()
(11, 7), max_length=4: set()
(11, 7), max_length=5: set()
(11, 7), max_length=6: set()
--------------------------------------------------
(11, 8), max_length=3: set()
(11, 8), max_length=4: set()
(11, 8), max_length=5: set()
(11, 8), max_length=6: {"L U' R L' U' R'", "L R U' L' U' R'", "B' U' F' B U' F", "B' U' F' U' F B", "B' U' B F' U' F", "B' U' F' U' B F", "R L U' L' U' R'", "L U' L' R U' R'"}
(11, 8): {"L U' R L' U' R'", "L R U' L' U' R'", "B' U' F' B U' F", "B' U' F' U' F B", "B' U' B F' U' F", "B' U' F' U' B F", "R L U' L' U' R'", "L U' L' R U' R'"}
--------------------------------------------------
(11, 9), max_length=3: set()
(11, 9), max_length=4: set()
(11, 9), max_length=5: set()
(11, 9), max_length=6: {"U' L F' U' F L'"}
(11, 9): {"U' L F' U' F L'"}
--------------------------------------------------
(11, 10), max_length=3: set()
(11, 10), max_length=4: set()
(11, 10), max_length=5: set()
(11, 10), max_length=6: {"B U' F' U' B' F", "B U B' F' U F", "B U' F' U' F B'", "B U F' B' U F", "B U' B' F' U' F", "B U' F' B' U' F"}
(11, 10): {"B U' F' U' B' F", "B U B' F' U F", "B U' F' U' F B'", "B U F' B' U F", "B U' B' F' U' F", "B U' F' B' U' F"}
--------------------------------------------------
(11, 11), max_length=3: set()
(11, 11), max_length=4: set()
(11, 11), max_length=5: set()
(11, 11), max_length=6: set()
--------------------------------------------------
(11, 12), max_length=3: set()
(11, 12), max_length=4: set()
(11, 12), max_length=5: set()
(11, 12), max_length=6: set()
--------------------------------------------------
(11, 13), max_length=3: set()
(11, 13), max_length=4: set()
(11, 13), max_length=5: set()
(11, 13), max_length=6: set()
--------------------------------------------------
(11, 14), max_length=3: set()
(11, 14), max_length=4: set()
(11, 14), max_length=5: {"U' L F2 L' F2"}
(11, 14): {"U' L F2 L' F2"}
--------------------------------------------------
(11, 15), max_length=3: set()
(11, 15), max_length=4: set()
(11, 15), max_length=5: set()
(11, 15), max_length=6: {"U' L' F' U' F L", "R U' F U' F' R'", "U' R' D' F' D R"}
(11, 15): {"U' L' F' U' F L", "R U' F U' F' R'", "U' R' D' F' D R"}
--------------------------------------------------
(12, 0), max_length=3: set()
(12, 0), max_length=4: set()
(12, 0), max_length=5: set()
(12, 0), max_length=6: {"B' U2 B R U R'"}
(12, 0): {"B' U2 B R U R'"}
--------------------------------------------------
(12, 1), max_length=3: set()
(12, 1), max_length=4: set()
(12, 1), max_length=5: set()
(12, 1), max_length=6: set()
--------------------------------------------------
(12, 2), max_length=3: set()
(12, 2), max_length=4: set()
(12, 2), max_length=5: set()
(12, 2), max_length=6: set()
--------------------------------------------------
(12, 3), max_length=3: set()
(12, 3), max_length=4: set()
(12, 3), max_length=5: set()
(12, 3), max_length=6: {"L U' L' F' U2 F", "F' L U' L' U2 F", "L U2 F' U' F L'"}
(12, 3): {"L U' L' F' U2 F", "F' L U' L' U2 F", "L U2 F' U' F L'"}
--------------------------------------------------
(12, 4), max_length=3: set()
(12, 4), max_length=4: set()
(12, 4), max_length=5: set()
(12, 4), max_length=6: {"B' U2 R U R' B", "R B' U B U2 R'", "B' U B R U2 R'"}
(12, 4): {"B' U2 R U R' B", "R B' U B U2 R'", "B' U B R U2 R'"}
--------------------------------------------------
(12, 5), max_length=3: set()
(12, 5), max_length=4: set()
(12, 5), max_length=5: set()
(12, 5), max_length=6: set()
--------------------------------------------------
(12, 6), max_length=3: set()
(12, 6), max_length=4: set()
(12, 6), max_length=5: set()
(12, 6), max_length=6: set()
--------------------------------------------------
(12, 7), max_length=3: set()
(12, 7), max_length=4: set()
(12, 7), max_length=5: set()
(12, 7), max_length=6: {"L U2 L' F' U' F"}
(12, 7): {"L U2 L' F' U' F"}
--------------------------------------------------
(12, 8), max_length=3: set()
(12, 8), max_length=4: set()
(12, 8), max_length=5: {'F2 L2 F L2 F', "R2 B2 R' B2 R'", "L R U2 L' R'", "B' F' U2 F B", "L R U2 R' L'", "R L U2 L' R'", "B' R2 B' R2 B2", "B' F' U2 B F", "F' B' U2 B F", "R L U2 R' L'", "F' B' U2 F B", 'L F2 L F2 L2'}
(12, 8): {'F2 L2 F L2 F', "R2 B2 R' B2 R'", "L R U2 L' R'", "B' F' U2 F B", "L R U2 R' L'", "R L U2 L' R'", "B' R2 B' R2 B2", "B' F' U2 B F", "F' B' U2 B F", "R L U2 R' L'", "F' B' U2 F B", 'L F2 L F2 L2'}
--------------------------------------------------
(12, 9), max_length=3: set()
(12, 9), max_length=4: set()
(12, 9), max_length=5: set()
(12, 9), max_length=6: set()
--------------------------------------------------
(12, 10), max_length=3: set()
(12, 10), max_length=4: set()
(12, 10), max_length=5: {"B' U2 R2 B R2"}
(12, 10): {"B' U2 R2 B R2"}
--------------------------------------------------
(12, 11), max_length=3: set()
(12, 11), max_length=4: set()
(12, 11), max_length=5: set()
(12, 11), max_length=6: set()
--------------------------------------------------
(12, 12), max_length=3: set()
(12, 12), max_length=4: set()
(12, 12), max_length=5: set()
(12, 12), max_length=6: set()
--------------------------------------------------
(12, 13), max_length=3: set()
(12, 13), max_length=4: set()
(12, 13), max_length=5: set()
(12, 13), max_length=6: set()
--------------------------------------------------
(12, 14), max_length=3: set()
(12, 14), max_length=4: set()
(12, 14), max_length=5: {"L U2 F2 L' F2"}
(12, 14): {"L U2 F2 L' F2"}
--------------------------------------------------
(12, 15), max_length=3: set()
(12, 15), max_length=4: set()
(12, 15), max_length=5: set()
(12, 15), max_length=6: set()
--------------------------------------------------
(13, 0), max_length=3: set()
(13, 0), max_length=4: set()
(13, 0), max_length=5: set()
(13, 0), max_length=6: {"U' B' R U2 R' B", "U' R B' U2 B R'", "R U' B' U2 B R'"}
(13, 0): {"U' B' R U2 R' B", "U' R B' U2 B R'", "R U' B' U2 B R'"}
--------------------------------------------------
(13, 1), max_length=3: set()
(13, 1), max_length=4: set()
(13, 1), max_length=5: set()
(13, 1), max_length=6: set()
--------------------------------------------------
(13, 2), max_length=3: set()
(13, 2), max_length=4: set()
(13, 2), max_length=5: set()
(13, 2), max_length=6: {"L2 U' R U L2 R'", "U2 B' R U2 R' B", "L2 U' R U R' L2", "R B' U R B R2", "U2 R B' U2 B R'"}
(13, 2): {"L2 U' R U L2 R'", "U2 B' R U2 R' B", "L2 U' R U R' L2", "R B' U R B R2", "U2 R B' U2 B R'"}
--------------------------------------------------
(13, 3), max_length=3: set()
(13, 3), max_length=4: set()
(13, 3), max_length=5: set()
(13, 3), max_length=6: {"B2 R B U R' B", "B' U2 F' B U' F", "B' U2 F' U' F B", "F' B2 U F U' B2", "B2 F' U F U' B2", "B' U2 F' U' B F", "B' U2 B F' U' F"}
(13, 3): {"B2 R B U R' B", "B' U2 F' B U' F", "B' U2 F' U' F B", "F' B2 U F U' B2", "B2 F' U F U' B2", "B' U2 F' U' B F", "B' U2 B F' U' F"}
--------------------------------------------------
(13, 4), max_length=3: set()
(13, 4), max_length=4: set()
(13, 4), max_length=5: set()
(13, 4), max_length=6: {"U R B' U2 B R'", "R U B' U2 B R'", "U B' R U2 R' B"}
(13, 4): {"U R B' U2 B R'", "R U B' U2 B R'", "U B' R U2 R' B"}
--------------------------------------------------
(13, 5), max_length=3: set()
(13, 5), max_length=4: set()
(13, 5), max_length=5: set()
(13, 5), max_length=6: set()
--------------------------------------------------
(13, 6), max_length=3: set()
(13, 6), max_length=4: set()
(13, 6), max_length=5: {"R B' U2 B R'", "B' R U2 R' B"}
(13, 6): {"R B' U2 B R'", "B' R U2 R' B"}
--------------------------------------------------
(13, 7), max_length=3: set()
(13, 7), max_length=4: set()
(13, 7), max_length=5: set()
(13, 7), max_length=6: set()
--------------------------------------------------
(13, 8), max_length=3: set()
(13, 8), max_length=4: set()
(13, 8), max_length=5: set()
(13, 8), max_length=6: set()
--------------------------------------------------
(13, 9), max_length=3: set()
(13, 9), max_length=4: set()
(13, 9), max_length=5: set()
(13, 9), max_length=6: set()
--------------------------------------------------
(13, 10), max_length=3: set()
(13, 10), max_length=4: set()
(13, 10), max_length=5: set()
(13, 10), max_length=6: {"B' R U R B R2", 'B2 R B R B R2'}
(13, 10): {"B' R U R B R2", 'B2 R B R B R2'}
--------------------------------------------------
(13, 11), max_length=3: set()
(13, 11), max_length=4: set()
(13, 11), max_length=5: set()
(13, 11), max_length=6: set()
--------------------------------------------------
(13, 12), max_length=3: set()
(13, 12), max_length=4: set()
(13, 12), max_length=5: set()
(13, 12), max_length=6: {"R2 U' L U L' R2", "R U2 B' U2 B R'", "B' F2 U B U' F2", "R2 U' L U R2 L'", "F2 B' U B U' F2"}
(13, 12): {"R2 U' L U L' R2", "R U2 B' U2 B R'", "B' F2 U B U' F2", "R2 U' L U R2 L'", "F2 B' U B U' F2"}
--------------------------------------------------
(13, 13), max_length=3: set()
(13, 13), max_length=4: set()
(13, 13), max_length=5: set()
(13, 13), max_length=6: set()
--------------------------------------------------
(13, 14), max_length=3: set()
(13, 14), max_length=4: set()
(13, 14), max_length=5: set()
(13, 14), max_length=6: set()
--------------------------------------------------
(13, 15), max_length=3: set()
(13, 15), max_length=4: set()
(13, 15), max_length=5: set()
(13, 15), max_length=6: set()
--------------------------------------------------
(14, 0), max_length=3: set()
(14, 0), max_length=4: set()
(14, 0), max_length=5: set()
(14, 0), max_length=6: set()
--------------------------------------------------
(14, 1), max_length=3: set()
(14, 1), max_length=4: set()
(14, 1), max_length=5: {"F' L U2 L' F", "L F' U2 F L'"}
(14, 1): {"F' L U2 L' F", "L F' U2 F L'"}
--------------------------------------------------
(14, 2), max_length=3: set()
(14, 2), max_length=4: set()
(14, 2), max_length=5: set()
(14, 2), max_length=6: set()
--------------------------------------------------
(14, 3), max_length=3: set()
(14, 3), max_length=4: set()
(14, 3), max_length=5: set()
(14, 3), max_length=6: {"F' U' L U2 L' F", "U' F' L U2 L' F", "U' L F' U2 F L'"}
(14, 3): {"F' U' L U2 L' F", "U' F' L U2 L' F", "U' L F' U2 F L'"}
--------------------------------------------------
(14, 4), max_length=3: set()
(14, 4), max_length=4: set()
(14, 4), max_length=5: set()
(14, 4), max_length=6: {"L U2 R L' U R'", "L U2 L' R U R'", "L U2 R U R' L'", "L2 R U' R' U L2", "L U2 R U L' R'", "L2 F' L' U' F L'", "R L2 U' R' U L2"}
(14, 4): {"L U2 R L' U R'", "L U2 L' R U R'", "L U2 R U R' L'", "L2 R U' R' U L2", "L U2 R U L' R'", "L2 F' L' U' F L'", "R L2 U' R' U L2"}
--------------------------------------------------
(14, 5), max_length=3: set()
(14, 5), max_length=4: set()
(14, 5), max_length=5: set()
(14, 5), max_length=6: {"U2 F' L U2 L' F", "U2 L F' U2 F L'", "B2 U F' U' B2 F", "B2 U F' U' F B2", "F' L U' F' L' F2"}
(14, 5): {"U2 F' L U2 L' F", "U2 L F' U2 F L'", "B2 U F' U' B2 F", "B2 U F' U' F B2", "F' L U' F' L' F2"}
--------------------------------------------------
(14, 6), max_length=3: set()
(14, 6), max_length=4: set()
(14, 6), max_length=5: set()
(14, 6), max_length=6: set()
--------------------------------------------------
(14, 7), max_length=3: set()
(14, 7), max_length=4: set()
(14, 7), max_length=5: set()
(14, 7), max_length=6: {"U L F' U2 F L'", "U F' L U2 L' F", "F' U L U2 L' F"}
(14, 7): {"U L F' U2 F L'", "U F' L U2 L' F", "F' U L U2 L' F"}
--------------------------------------------------
(14, 8), max_length=3: set()
(14, 8), max_length=4: set()
(14, 8), max_length=5: set()
(14, 8), max_length=6: set()
--------------------------------------------------
(14, 9), max_length=3: set()
(14, 9), max_length=4: set()
(14, 9), max_length=5: set()
(14, 9), max_length=6: set()
--------------------------------------------------
(14, 10), max_length=3: set()
(14, 10), max_length=4: set()
(14, 10), max_length=5: set()
(14, 10), max_length=6: set()
--------------------------------------------------
(14, 11), max_length=3: set()
(14, 11), max_length=4: set()
(14, 11), max_length=5: set()
(14, 11), max_length=6: set()
--------------------------------------------------
(14, 12), max_length=3: set()
(14, 12), max_length=4: set()
(14, 12), max_length=5: set()
(14, 12), max_length=6: {"F2 U B' U' F2 B", "F2 U B' U' B F2", "R2 L U' L' U R2", "F' U2 L U2 L' F", "L R2 U' L' U R2"}
(14, 12): {"F2 U B' U' F2 B", "F2 U B' U' B F2", "R2 L U' L' U R2", "F' U2 L U2 L' F", "L R2 U' L' U R2"}
--------------------------------------------------
(14, 13), max_length=3: set()
(14, 13), max_length=4: set()
(14, 13), max_length=5: set()
(14, 13), max_length=6: set()
--------------------------------------------------
(14, 14), max_length=3: set()
(14, 14), max_length=4: set()
(14, 14), max_length=5: set()
(14, 14), max_length=6: {"L F' U' F' L' F2", "L2 F' L' F' L' F2"}
(14, 14): {"L F' U' F' L' F2", "L2 F' L' F' L' F2"}
--------------------------------------------------
(14, 15), max_length=3: set()
(14, 15), max_length=4: set()
(14, 15), max_length=5: set()
(14, 15), max_length=6: set()
--------------------------------------------------
(15, 0), max_length=3: set()
(15, 0), max_length=4: set()
(15, 0), max_length=5: set()
(15, 0), max_length=6: set()
--------------------------------------------------
(15, 1), max_length=3: set()
(15, 1), max_length=4: set()
(15, 1), max_length=5: set()
(15, 1), max_length=6: set()
--------------------------------------------------
(15, 2), max_length=3: set()
(15, 2), max_length=4: set()
(15, 2), max_length=5: set()
(15, 2), max_length=6: set()
--------------------------------------------------
(15, 3), max_length=3: set()
(15, 3), max_length=4: set()
(15, 3), max_length=5: set()
(15, 3), max_length=6: set()
--------------------------------------------------
(15, 4), max_length=3: set()
(15, 4), max_length=4: set()
(15, 4), max_length=5: set()
(15, 4), max_length=6: set()
--------------------------------------------------
(15, 5), max_length=3: set()
(15, 5), max_length=4: set()
(15, 5), max_length=5: set()
(15, 5), max_length=6: {"B U F' B' U' F", "B U' F' B' U F", "B U B' F' U' F", "B U' B' F' U F", "B U F' U' F B'", "B U F' U' B' F"}
(15, 5): {"B U F' B' U' F", "B U' F' B' U F", "B U B' F' U' F", "B U' B' F' U F", "B U F' U' F B'", "B U F' U' B' F"}
--------------------------------------------------
(15, 6), max_length=3: set()
(15, 6), max_length=4: set()
(15, 6), max_length=5: set()
(15, 6), max_length=6: set()
--------------------------------------------------
(15, 7), max_length=3: set()
(15, 7), max_length=4: set()
(15, 7), max_length=5: set()
(15, 7), max_length=6: set()
--------------------------------------------------
(15, 8), max_length=3: set()
(15, 8), max_length=4: set()
(15, 8), max_length=5: set()
(15, 8), max_length=6: set()
--------------------------------------------------
(15, 9), max_length=3: set()
(15, 9), max_length=4: set()
(15, 9), max_length=5: set()
(15, 9), max_length=6: set()
--------------------------------------------------
(15, 10), max_length=3: set()
(15, 10), max_length=4: set()
(15, 10), max_length=5: set()
(15, 10), max_length=6: set()
--------------------------------------------------
(15, 11), max_length=3: set()
(15, 11), max_length=4: set()
(15, 11), max_length=5: set()
(15, 11), max_length=6: {"B' R B2 U B' R'", "R' F' U F2 R F'", "F' R' U' R U2 F", "B U' R U2 R' B'", "B U2 R U' R' B'", "R' U' R F' U2 F", "F' R' U2 R U' F", "F R2 F' R' U' R'", "B U2 B' R U' R'", "R' U' R' B' R2 B"}
(15, 11): {"B' R B2 U B' R'", "R' F' U F2 R F'", "F' R' U' R U2 F", "B U' R U2 R' B'", "B U2 R U' R' B'", "R' U' R F' U2 F", "F' R' U2 R U' F", "F R2 F' R' U' R'", "B U2 B' R U' R'", "R' U' R' B' R2 B"}
--------------------------------------------------
(15, 12), max_length=3: set()
(15, 12), max_length=4: set()
(15, 12), max_length=5: set()
(15, 12), max_length=6: {"B F' U' B' U F", "F' B U' B' U F"}
(15, 12): {"B F' U' B' U F", "F' B U' B' U F"}
--------------------------------------------------
(15, 13), max_length=3: set()
(15, 13), max_length=4: set()
(15, 13), max_length=5: set()
(15, 13), max_length=6: set()
--------------------------------------------------
(15, 14), max_length=3: set()
(15, 14), max_length=4: set()
(15, 14), max_length=5: set()
(15, 14), max_length=6: set()
--------------------------------------------------
(15, 15), max_length=3: set()
(15, 15), max_length=4: set()
(15, 15), max_length=5: set()
(15, 15), max_length=6: set()
--------------------------------------------------
(16, 0), max_length=3: set()
(16, 0), max_length=4: set()
(16, 0), max_length=5: {"R' U2 R2 U' R'", "R' U' R2 U2 R'"}
(16, 0): {"R' U2 R2 U' R'", "R' U' R2 U2 R'"}
--------------------------------------------------
(16, 1), max_length=3: set()
(16, 1), max_length=4: set()
(16, 1), max_length=5: set()
(16, 1), max_length=6: set()
--------------------------------------------------
(16, 2), max_length=3: set()
(16, 2), max_length=4: set()
(16, 2), max_length=5: set()
(16, 2), max_length=6: {"U' R' U2 R2 U' R'", "U' R' U' R2 U2 R'"}
(16, 2): {"U' R' U2 R2 U' R'", "U' R' U' R2 U2 R'"}
--------------------------------------------------
(16, 3), max_length=3: set()
(16, 3), max_length=4: set()
(16, 3), max_length=5: set()
(16, 3), max_length=6: set()
--------------------------------------------------
(16, 4), max_length=3: set()
(16, 4), max_length=4: set()
(16, 4), max_length=5: set()
(16, 4), max_length=6: {"U2 R' U2 R2 U' R'", "U2 R' U' R2 U2 R'"}
(16, 4): {"U2 R' U2 R2 U' R'", "U2 R' U' R2 U2 R'"}
--------------------------------------------------
(16, 5), max_length=3: set()
(16, 5), max_length=4: set()
(16, 5), max_length=5: set()
(16, 5), max_length=6: set()
--------------------------------------------------
(16, 6), max_length=3: set()
(16, 6), max_length=4: set()
(16, 6), max_length=5: set()
(16, 6), max_length=6: {"U R' U2 R2 U' R'", "U R' U' R2 U2 R'"}
(16, 6): {"U R' U2 R2 U' R'", "U R' U' R2 U2 R'"}
--------------------------------------------------
(16, 7), max_length=3: set()
(16, 7), max_length=4: set()
(16, 7), max_length=5: set()
(16, 7), max_length=6: set()
--------------------------------------------------
(16, 8), max_length=3: set()
(16, 8), max_length=4: set()
(16, 8), max_length=5: set()
(16, 8), max_length=6: set()
--------------------------------------------------
(16, 9), max_length=3: set()
(16, 9), max_length=4: set()
(16, 9), max_length=5: set()
(16, 9), max_length=6: set()
--------------------------------------------------
(16, 10), max_length=3: set()
(16, 10), max_length=4: set()
(16, 10), max_length=5: set()
(16, 10), max_length=6: set()
--------------------------------------------------
(16, 11), max_length=3: set()
(16, 11), max_length=4: set()
(16, 11), max_length=5: set()
(16, 11), max_length=6: set()
--------------------------------------------------
(16, 12), max_length=3: set()
(16, 12), max_length=4: set()
(16, 12), max_length=5: set()
(16, 12), max_length=6: set()
--------------------------------------------------
(16, 13), max_length=3: set()
(16, 13), max_length=4: set()
(16, 13), max_length=5: set()
(16, 13), max_length=6: {"B' R2 B R' U R'"}
(16, 13): {"B' R2 B R' U R'"}
--------------------------------------------------
(16, 14), max_length=3: set()
(16, 14), max_length=4: set()
(16, 14), max_length=5: set()
(16, 14), max_length=6: {"F' R' U2 F2 R F"}
(16, 14): {"F' R' U2 F2 R F"}
--------------------------------------------------
(16, 15), max_length=3: set()
(16, 15), max_length=4: set()
(16, 15), max_length=5: set()
(16, 15), max_length=6: set()
--------------------------------------------------
(17, 0), max_length=3: set()
(17, 0), max_length=4: set()
(17, 0), max_length=5: set()
(17, 0), max_length=6: {"B U B' R U R'"}
(17, 0): {"B U B' R U R'"}
--------------------------------------------------
(17, 1), max_length=3: set()
(17, 1), max_length=4: set()
(17, 1), max_length=5: set()
(17, 1), max_length=6: {"U B F' U F B'", "U F' B U B' F", "F' U B U B' F", "U B F' U B' F", "U F' B U F B'", "F' U B U F B'"}
(17, 1): {"U B F' U F B'", "U F' B U B' F", "F' U B U B' F", "U B F' U B' F", "U F' B U F B'", "F' U B U F B'"}
--------------------------------------------------
(17, 2), max_length=3: set()
(17, 2), max_length=4: set()
(17, 2), max_length=5: set()
(17, 2), max_length=6: set()
--------------------------------------------------
(17, 3), max_length=3: set()
(17, 3), max_length=4: set()
(17, 3), max_length=5: {"F' B U F B'", "F' B U B' F", "B F' U F B'", "B F' U B' F"}
(17, 3): {"F' B U F B'", "F' B U B' F", "B F' U F B'", "B F' U B' F"}
--------------------------------------------------
(17, 4), max_length=3: set()
(17, 4), max_length=4: set()
(17, 4), max_length=5: set()
(17, 4), max_length=6: set()
--------------------------------------------------
(17, 5), max_length=3: set()
(17, 5), max_length=4: set()
(17, 5), max_length=5: set()
(17, 5), max_length=6: {"U' B F' U B' F", "U' F' B U F B'", "U' B F' U F B'", "U' F' B U B' F"}
(17, 5): {"U' B F' U B' F", "U' F' B U F B'", "U' B F' U F B'", "U' F' B U B' F"}
--------------------------------------------------
(17, 6), max_length=3: set()
(17, 6), max_length=4: set()
(17, 6), max_length=5: set()
(17, 6), max_length=6: {"B U R U R' B'"}
(17, 6): {"B U R U R' B'"}
--------------------------------------------------
(17, 7), max_length=3: set()
(17, 7), max_length=4: set()
(17, 7), max_length=5: set()
(17, 7), max_length=6: {"U2 F' B U F B'", "U2 F' B U B' F", "U2 B F' U B' F", 'B U B2 R2 B R2', "F' U2 B U F B'", "F' U2 B U B' F", "U2 B F' U F B'"}
(17, 7): {"U2 F' B U F B'", "U2 F' B U B' F", "U2 B F' U B' F", 'B U B2 R2 B R2', "F' U2 B U F B'", "F' U2 B U B' F", "U2 B F' U F B'"}
--------------------------------------------------
(17, 8), max_length=3: set()
(17, 8), max_length=4: set()
(17, 8), max_length=5: set()
(17, 8), max_length=6: {"B R' B R B' R", "B U R2 B R2 B'"}
(17, 8): {"B R' B R B' R", "B U R2 B R2 B'"}
--------------------------------------------------
(17, 9), max_length=3: set()
(17, 9), max_length=4: set()
(17, 9), max_length=5: set()
(17, 9), max_length=6: set()
--------------------------------------------------
(17, 10), max_length=3: set()
(17, 10), max_length=4: set()
(17, 10), max_length=5: set()
(17, 10), max_length=6: set()
--------------------------------------------------
(17, 11), max_length=3: set()
(17, 11), max_length=4: set()
(17, 11), max_length=5: set()
(17, 11), max_length=6: set()
--------------------------------------------------
(17, 12), max_length=3: set()
(17, 12), max_length=4: set()
(17, 12), max_length=5: set()
(17, 12), max_length=6: {"F' U' B U F B'", "F' U' B U B' F"}
(17, 12): {"F' U' B U F B'", "F' U' B U B' F"}
--------------------------------------------------
(17, 13), max_length=3: set()
(17, 13), max_length=4: set()
(17, 13), max_length=5: set()
(17, 13), max_length=6: set()
--------------------------------------------------
(17, 14), max_length=3: set()
(17, 14), max_length=4: set()
(17, 14), max_length=5: set()
(17, 14), max_length=6: {"L R2 F2 L' R2 F2", "L R2 F2 R2 L' F2", "R2 L F2 L' R2 F2", "R2 L F2 R2 L' F2"}
(17, 14): {"L R2 F2 L' R2 F2", "L R2 F2 R2 L' F2", "R2 L F2 L' R2 F2", "R2 L F2 R2 L' F2"}
--------------------------------------------------
(17, 15), max_length=3: set()
(17, 15), max_length=4: set()
(17, 15), max_length=5: set()
(17, 15), max_length=6: set()
--------------------------------------------------
(18, 0), max_length=3: set()
(18, 0), max_length=4: set()
(18, 0), max_length=5: set()
(18, 0), max_length=6: set()
--------------------------------------------------
(18, 1), max_length=3: set()
(18, 1), max_length=4: set()
(18, 1), max_length=5: set()
(18, 1), max_length=6: set()
--------------------------------------------------
(18, 2), max_length=3: set()
(18, 2), max_length=4: set()
(18, 2), max_length=5: set()
(18, 2), max_length=6: set()
--------------------------------------------------
(18, 3), max_length=3: set()
(18, 3), max_length=4: set()
(18, 3), max_length=5: set()
(18, 3), max_length=6: set()
--------------------------------------------------
(18, 4), max_length=3: set()
(18, 4), max_length=4: set()
(18, 4), max_length=5: set()
(18, 4), max_length=6: set()
--------------------------------------------------
(18, 5), max_length=3: set()
(18, 5), max_length=4: set()
(18, 5), max_length=5: set()
(18, 5), max_length=6: set()
--------------------------------------------------
(18, 6), max_length=3: set()
(18, 6), max_length=4: set()
(18, 6), max_length=5: set()
(18, 6), max_length=6: set()
--------------------------------------------------
(18, 7), max_length=3: set()
(18, 7), max_length=4: set()
(18, 7), max_length=5: set()
(18, 7), max_length=6: set()
--------------------------------------------------
(18, 8), max_length=3: set()
(18, 8), max_length=4: set()
(18, 8), max_length=5: {"L2 D F2 D' L2", "B2 D' R2 D B2"}
(18, 8): {"L2 D F2 D' L2", "B2 D' R2 D B2"}
--------------------------------------------------
(18, 9), max_length=3: set()
(18, 9), max_length=4: set()
(18, 9), max_length=5: set()
(18, 9), max_length=6: {"R2 B' R' U B R'", "F2 L F U' L' F"}
(18, 9): {"R2 B' R' U B R'", "F2 L F U' L' F"}
--------------------------------------------------
(18, 10), max_length=3: set()
(18, 10), max_length=4: set()
(18, 10), max_length=5: {'R2 U2 R2 U2 R2', "R' B' R2 B R", "R2 U' R2 U R2", 'L2 D2 R2 D2 L2'}
(18, 10): {'R2 U2 R2 U2 R2', "R' B' R2 B R", "R2 U' R2 U R2", 'L2 D2 R2 D2 L2'}
--------------------------------------------------
(18, 11), max_length=3: set()
(18, 11), max_length=4: set()
(18, 11), max_length=5: {"B' D' R D B", "F' R' U R F"}
(18, 11): {"B' D' R D B", "F' R' U R F"}
--------------------------------------------------
(18, 12), max_length=3: {''}
(18, 12): {''}
--------------------------------------------------
(18, 13), max_length=3: set()
(18, 13), max_length=4: set()
(18, 13), max_length=5: set()
(18, 13), max_length=6: set()
--------------------------------------------------
(18, 14), max_length=3: set()
(18, 14), max_length=4: set()
(18, 14), max_length=5: {"F L F2 L' F'", 'B2 D2 F2 D2 B2', 'F2 U2 F2 U2 F2', "F2 U F2 U' F2"}
(18, 14): {"F L F2 L' F'", 'B2 D2 F2 D2 B2', 'F2 U2 F2 U2 F2', "F2 U F2 U' F2"}
--------------------------------------------------
(18, 15), max_length=3: set()
(18, 15), max_length=4: set()
(18, 15), max_length=5: {"R F U' F' R'", "L D F' D' L'"}
(18, 15): {"R F U' F' R'", "L D F' D' L'"}
--------------------------------------------------
(19, 0), max_length=3: set()
(19, 0), max_length=4: set()
(19, 0), max_length=5: set()
(19, 0), max_length=6: {"R B U2 B' U R'"}
(19, 0): {"R B U2 B' U R'"}
--------------------------------------------------
(19, 1), max_length=3: set()
(19, 1), max_length=4: set()
(19, 1), max_length=5: set()
(19, 1), max_length=6: {"R U' R' F' U' F", "F2 L F L' U2 F"}
(19, 1): {"R U' R' F' U' F", "F2 L F L' U2 F"}
--------------------------------------------------
(19, 2), max_length=3: set()
(19, 2), max_length=4: set()
(19, 2), max_length=5: set()
(19, 2), max_length=6: {"R U2 B' R B R2", "F' U2 F R U2 R'", "R U' R2 F R F'"}
(19, 2): {"R U2 B' R B R2", "F' U2 F R U2 R'", "R U' R2 F R F'"}
--------------------------------------------------
(19, 3), max_length=3: set()
(19, 3), max_length=4: set()
(19, 3), max_length=5: set()
(19, 3), max_length=6: set()
--------------------------------------------------
(19, 4), max_length=3: set()
(19, 4), max_length=4: set()
(19, 4), max_length=5: set()
(19, 4), max_length=6: set()
--------------------------------------------------
(19, 5), max_length=3: set()
(19, 5), max_length=4: set()
(19, 5), max_length=5: set()
(19, 5), max_length=6: set()
--------------------------------------------------
(19, 6), max_length=3: set()
(19, 6), max_length=4: set()
(19, 6), max_length=5: set()
(19, 6), max_length=6: set()
--------------------------------------------------
(19, 7), max_length=3: set()
(19, 7), max_length=4: set()
(19, 7), max_length=5: set()
(19, 7), max_length=6: {"F' U L' U2 L F", "R' F R F2 U' F"}
(19, 7): {"F' U L' U2 L F", "R' F R F2 U' F"}
--------------------------------------------------
(19, 8), max_length=3: set()
(19, 8), max_length=4: set()
(19, 8), max_length=5: set()
(19, 8), max_length=6: set()
--------------------------------------------------
(19, 9), max_length=3: set()
(19, 9), max_length=4: set()
(19, 9), max_length=5: set()
(19, 9), max_length=6: {"F' L U L' U2 F", "R U2 B' U B R'"}
(19, 9): {"F' L U L' U2 F", "R U2 B' U B R'"}
--------------------------------------------------
(19, 10), max_length=3: set()
(19, 10), max_length=4: set()
(19, 10), max_length=5: set()
(19, 10), max_length=6: set()
--------------------------------------------------
(19, 11), max_length=3: set()
(19, 11), max_length=4: set()
(19, 11), max_length=5: set()
(19, 11), max_length=6: set()
--------------------------------------------------
(19, 12), max_length=3: set()
(19, 12), max_length=4: set()
(19, 12), max_length=5: set()
(19, 12), max_length=6: set()
--------------------------------------------------
(19, 13), max_length=3: set()
(19, 13), max_length=4: set()
(19, 13), max_length=5: set()
(19, 13), max_length=6: set()
--------------------------------------------------
(19, 14), max_length=3: set()
(19, 14), max_length=4: set()
(19, 14), max_length=5: set()
(19, 14), max_length=6: {"F' U R' F2 R F", "R U L' U2 L R'", "R U L' U2 R' L"}
(19, 14): {"F' U R' F2 R F", "R U L' U2 L R'", "R U L' U2 R' L"}
--------------------------------------------------
(19, 15), max_length=3: set()
(19, 15), max_length=4: set()
(19, 15), max_length=5: set()
(19, 15), max_length=6: set()
--------------------------------------------------
(20, 0), max_length=3: set()
(20, 0), max_length=4: set()
(20, 0), max_length=5: set()
(20, 0), max_length=6: {"R U' B U2 B' R'", "F R' F' R2 U R'"}
(20, 0): {"R U' B U2 B' R'", "F R' F' R2 U R'"}
--------------------------------------------------
(20, 1), max_length=3: set()
(20, 1), max_length=4: set()
(20, 1), max_length=5: set()
(20, 1), max_length=6: set()
--------------------------------------------------
(20, 2), max_length=3: set()
(20, 2), max_length=4: set()
(20, 2), max_length=5: set()
(20, 2), max_length=6: set()
--------------------------------------------------
(20, 3), max_length=3: set()
(20, 3), max_length=4: set()
(20, 3), max_length=5: set()
(20, 3), max_length=6: set()
--------------------------------------------------
(20, 4), max_length=3: set()
(20, 4), max_length=4: set()
(20, 4), max_length=5: set()
(20, 4), max_length=6: set()
--------------------------------------------------
(20, 5), max_length=3: set()
(20, 5), max_length=4: set()
(20, 5), max_length=5: set()
(20, 5), max_length=6: {"F' U F2 R' F' R", "F' U2 L F' L' F2", "R U2 R' F' U2 F"}
(20, 5): {"F' U F2 R' F' R", "F' U2 L F' L' F2", "R U2 R' F' U2 F"}
--------------------------------------------------
(20, 6), max_length=3: set()
(20, 6), max_length=4: set()
(20, 6), max_length=5: set()
(20, 6), max_length=6: {"F' U F R U R'", "R2 B' R' B U2 R'"}
(20, 6): {"F' U F R U R'", "R2 B' R' B U2 R'"}
--------------------------------------------------
(20, 7), max_length=3: set()
(20, 7), max_length=4: set()
(20, 7), max_length=5: set()
(20, 7), max_length=6: {"F' L' U2 L U' F"}
(20, 7): {"F' L' U2 L U' F"}
--------------------------------------------------
(20, 8), max_length=3: set()
(20, 8), max_length=4: set()
(20, 8), max_length=5: set()
(20, 8), max_length=6: set()
--------------------------------------------------
(20, 9), max_length=3: set()
(20, 9), max_length=4: set()
(20, 9), max_length=5: set()
(20, 9), max_length=6: {"R B' U' B U2 R'", "F' U2 L U' L' F"}
(20, 9): {"R B' U' B U2 R'", "F' U2 L U' L' F"}
--------------------------------------------------
(20, 10), max_length=3: set()
(20, 10), max_length=4: set()
(20, 10), max_length=5: set()
(20, 10), max_length=6: {"R U' F R2 F' R'", "F' U' B U2 B' F", "F' U' B U2 F B'"}
(20, 10): {"R U' F R2 F' R'", "F' U' B U2 B' F", "F' U' B U2 F B'"}
--------------------------------------------------
(20, 11), max_length=3: set()
(20, 11), max_length=4: set()
(20, 11), max_length=5: set()
(20, 11), max_length=6: set()
--------------------------------------------------
(20, 12), max_length=3: set()
(20, 12), max_length=4: set()
(20, 12), max_length=5: set()
(20, 12), max_length=6: set()
--------------------------------------------------
(20, 13), max_length=3: set()
(20, 13), max_length=4: set()
(20, 13), max_length=5: set()
(20, 13), max_length=6: set()
--------------------------------------------------
(20, 14), max_length=3: set()
(20, 14), max_length=4: set()
(20, 14), max_length=5: set()
(20, 14), max_length=6: set()
--------------------------------------------------
(20, 15), max_length=3: set()
(20, 15), max_length=4: set()
(20, 15), max_length=5: set()
(20, 15), max_length=6: set()
--------------------------------------------------
(21, 0), max_length=3: set()
(21, 0), max_length=4: set()
(21, 0), max_length=5: set()
(21, 0), max_length=6: set()
--------------------------------------------------
(21, 1), max_length=3: set()
(21, 1), max_length=4: set()
(21, 1), max_length=5: set()
(21, 1), max_length=6: set()
--------------------------------------------------
(21, 2), max_length=3: set()
(21, 2), max_length=4: set()
(21, 2), max_length=5: set()
(21, 2), max_length=6: {"L' U' R U R' L", "L' U' R U L R'", "L' U' R L U R'", "L' U' L R U R'", "L' U L R U' R'", "L' U R L U' R'"}
(21, 2): {"L' U' R U R' L", "L' U' R U L R'", "L' U' R L U R'", "L' U' L R U R'", "L' U L R U' R'", "L' U R L U' R'"}
--------------------------------------------------
(21, 3), max_length=3: set()
(21, 3), max_length=4: set()
(21, 3), max_length=5: set()
(21, 3), max_length=6: set()
--------------------------------------------------
(21, 4), max_length=3: set()
(21, 4), max_length=4: set()
(21, 4), max_length=5: set()
(21, 4), max_length=6: set()
--------------------------------------------------
(21, 5), max_length=3: set()
(21, 5), max_length=4: set()
(21, 5), max_length=5: set()
(21, 5), max_length=6: set()
--------------------------------------------------
(21, 6), max_length=3: set()
(21, 6), max_length=4: set()
(21, 6), max_length=5: set()
(21, 6), max_length=6: set()
--------------------------------------------------
(21, 7), max_length=3: set()
(21, 7), max_length=4: set()
(21, 7), max_length=5: set()
(21, 7), max_length=6: set()
--------------------------------------------------
(21, 8), max_length=3: set()
(21, 8), max_length=4: set()
(21, 8), max_length=5: set()
(21, 8), max_length=6: set()
--------------------------------------------------
(21, 9), max_length=3: set()
(21, 9), max_length=4: set()
(21, 9), max_length=5: set()
(21, 9), max_length=6: set()
--------------------------------------------------
(21, 10), max_length=3: set()
(21, 10), max_length=4: set()
(21, 10), max_length=5: set()
(21, 10), max_length=6: set()
--------------------------------------------------
(21, 11), max_length=3: set()
(21, 11), max_length=4: set()
(21, 11), max_length=5: set()
(21, 11), max_length=6: set()
--------------------------------------------------
(21, 12), max_length=3: set()
(21, 12), max_length=4: set()
(21, 12), max_length=5: set()
(21, 12), max_length=6: {"R L' U L U' R'", "L' R U L U' R'"}
(21, 12): {"R L' U L U' R'", "L' R U L U' R'"}
--------------------------------------------------
(21, 13), max_length=3: set()
(21, 13), max_length=4: set()
(21, 13), max_length=5: set()
(21, 13), max_length=6: set()
--------------------------------------------------
(21, 14), max_length=3: set()
(21, 14), max_length=4: set()
(21, 14), max_length=5: set()
(21, 14), max_length=6: set()
--------------------------------------------------
(21, 15), max_length=3: set()
(21, 15), max_length=4: set()
(21, 15), max_length=5: set()
(21, 15), max_length=6: {"R F U F' U2 R'", "F R U' R2 F' R", "L' U2 L F' U F", "L' U2 F' U F L", "R F U2 F' U R'", "F U F L F2 L'", "R' F2 R F U F", "L F' L2 U' L F", "F U F' R U2 R'", "L' U F' U2 F L"}
(21, 15): {"R F U F' U2 R'", "F R U' R2 F' R", "L' U2 L F' U F", "L' U2 F' U F L", "R F U2 F' U R'", "F U F L F2 L'", "R' F2 R F U F", "L F' L2 U' L F", "F U F' R U2 R'", "L' U F' U2 F L"}
--------------------------------------------------
(22, 0), max_length=3: set()
(22, 0), max_length=4: set()
(22, 0), max_length=5: set()
(22, 0), max_length=6: {"L' U' L2 F2 L' F2", "U2 R L' U' L R'", "R U2 L' U' R' L", "U2 L' R U' R' L", "U2 L' R U' L R'", "R U2 L' U' L R'", "U2 R L' U' R' L"}
(22, 0): {"L' U' L2 F2 L' F2", "U2 R L' U' L R'", "R U2 L' U' R' L", "U2 L' R U' R' L", "U2 L' R U' L R'", "R U2 L' U' L R'", "U2 R L' U' R' L"}
--------------------------------------------------
(22, 1), max_length=3: set()
(22, 1), max_length=4: set()
(22, 1), max_length=5: set()
(22, 1), max_length=6: {"L' U' F' U' F L"}
(22, 1): {"L' U' F' U' F L"}
--------------------------------------------------
(22, 2), max_length=3: set()
(22, 2), max_length=4: set()
(22, 2), max_length=5: set()
(22, 2), max_length=6: {"U R L' U' R' L", "U L' R U' L R'", "U L' R U' R' L", "U R L' U' L R'"}
(22, 2): {"U R L' U' R' L", "U L' R U' L R'", "U L' R U' R' L", "U R L' U' L R'"}
--------------------------------------------------
(22, 3), max_length=3: set()
(22, 3), max_length=4: set()
(22, 3), max_length=5: set()
(22, 3), max_length=6: set()
--------------------------------------------------
(22, 4), max_length=3: set()
(22, 4), max_length=4: set()
(22, 4), max_length=5: {"L' R U' R' L", "R L' U' L R'", "R L' U' R' L", "L' R U' L R'"}
(22, 4): {"L' R U' R' L", "R L' U' L R'", "R L' U' R' L", "L' R U' L R'"}
--------------------------------------------------
(22, 5), max_length=3: set()
(22, 5), max_length=4: set()
(22, 5), max_length=5: set()
(22, 5), max_length=6: set()
--------------------------------------------------
(22, 6), max_length=3: set()
(22, 6), max_length=4: set()
(22, 6), max_length=5: set()
(22, 6), max_length=6: {"U' R L' U' L R'", "R U' L' U' R' L", "R U' L' U' L R'", "U' L' R U' R' L", "U' R L' U' R' L", "U' L' R U' L R'"}
(22, 6): {"U' R L' U' L R'", "R U' L' U' R' L", "R U' L' U' L R'", "U' L' R U' R' L", "U' R L' U' R' L", "U' L' R U' L R'"}
--------------------------------------------------
(22, 7), max_length=3: set()
(22, 7), max_length=4: set()
(22, 7), max_length=5: set()
(22, 7), max_length=6: {"L' U' L F' U' F"}
(22, 7): {"L' U' L F' U' F"}
--------------------------------------------------
(22, 8), max_length=3: set()
(22, 8), max_length=4: set()
(22, 8), max_length=5: set()
(22, 8), max_length=6: {"L' F L' F' L F'", "L' U' F2 L' F2 L"}
(22, 8): {"L' F L' F' L F'", "L' U' F2 L' F2 L"}
--------------------------------------------------
(22, 9), max_length=3: set()
(22, 9), max_length=4: set()
(22, 9), max_length=5: set()
(22, 9), max_length=6: set()
--------------------------------------------------
(22, 10), max_length=3: set()
(22, 10), max_length=4: set()
(22, 10), max_length=5: set()
(22, 10), max_length=6: {"F2 B' R2 B F2 R2", "B' F2 R2 B F2 R2", "B' F2 R2 F2 B R2", "F2 B' R2 F2 B R2"}
(22, 10): {"F2 B' R2 B F2 R2", "B' F2 R2 B F2 R2", "B' F2 R2 F2 B R2", "F2 B' R2 F2 B R2"}
--------------------------------------------------
(22, 11), max_length=3: set()
(22, 11), max_length=4: set()
(22, 11), max_length=5: set()
(22, 11), max_length=6: set()
--------------------------------------------------
(22, 12), max_length=3: set()
(22, 12), max_length=4: set()
(22, 12), max_length=5: set()
(22, 12), max_length=6: {"R U L' U' L R'", "R U L' U' R' L"}
(22, 12): {"R U L' U' L R'", "R U L' U' R' L"}
--------------------------------------------------
(22, 13), max_length=3: set()
(22, 13), max_length=4: set()
(22, 13), max_length=5: set()
(22, 13), max_length=6: set()
--------------------------------------------------
(22, 14), max_length=3: set()
(22, 14), max_length=4: set()
(22, 14), max_length=5: set()
(22, 14), max_length=6: set()
--------------------------------------------------
(22, 15), max_length=3: set()
(22, 15), max_length=4: set()
(22, 15), max_length=5: set()
(22, 15), max_length=6: set()
--------------------------------------------------
(23, 0), max_length=3: set()
(23, 0), max_length=4: set()
(23, 0), max_length=5: set()
(23, 0), max_length=6: set()
--------------------------------------------------
(23, 1), max_length=3: set()
(23, 1), max_length=4: set()
(23, 1), max_length=5: set()
(23, 1), max_length=6: {"U' F U2 F2 U F", "U' F U F2 U2 F"}
(23, 1): {"U' F U2 F2 U F", "U' F U F2 U2 F"}
--------------------------------------------------
(23, 2), max_length=3: set()
(23, 2), max_length=4: set()
(23, 2), max_length=5: set()
(23, 2), max_length=6: set()
--------------------------------------------------
(23, 3), max_length=3: set()
(23, 3), max_length=4: set()
(23, 3), max_length=5: set()
(23, 3), max_length=6: {'U2 F U2 F2 U F', 'U2 F U F2 U2 F'}
(23, 3): {'U2 F U2 F2 U F', 'U2 F U F2 U2 F'}
--------------------------------------------------
(23, 4), max_length=3: set()
(23, 4), max_length=4: set()
(23, 4), max_length=5: set()
(23, 4), max_length=6: set()
--------------------------------------------------
(23, 5), max_length=3: set()
(23, 5), max_length=4: set()
(23, 5), max_length=5: set()
(23, 5), max_length=6: {'U F U2 F2 U F', 'U F U F2 U2 F'}
(23, 5): {'U F U2 F2 U F', 'U F U F2 U2 F'}
--------------------------------------------------
(23, 6), max_length=3: set()
(23, 6), max_length=4: set()
(23, 6), max_length=5: set()
(23, 6), max_length=6: set()
--------------------------------------------------
(23, 7), max_length=3: set()
(23, 7), max_length=4: set()
(23, 7), max_length=5: {'F U F2 U2 F', 'F U2 F2 U F'}
(23, 7): {'F U F2 U2 F', 'F U2 F2 U F'}
--------------------------------------------------
(23, 8), max_length=3: set()
(23, 8), max_length=4: set()
(23, 8), max_length=5: set()
(23, 8), max_length=6: set()
--------------------------------------------------
(23, 9), max_length=3: set()
(23, 9), max_length=4: set()
(23, 9), max_length=5: set()
(23, 9), max_length=6: set()
--------------------------------------------------
(23, 10), max_length=3: set()
(23, 10), max_length=4: set()
(23, 10), max_length=5: set()
(23, 10), max_length=6: {"R F U2 R2 F' R'"}
(23, 10): {"R F U2 R2 F' R'"}
--------------------------------------------------
(23, 11), max_length=3: set()
(23, 11), max_length=4: set()
(23, 11), max_length=5: set()
(23, 11), max_length=6: set()
--------------------------------------------------
(23, 12), max_length=3: set()
(23, 12), max_length=4: set()
(23, 12), max_length=5: set()
(23, 12), max_length=6: set()
--------------------------------------------------
(23, 13), max_length=3: set()
(23, 13), max_length=4: set()
(23, 13), max_length=5: set()
(23, 13), max_length=6: {"L F2 L' F U' F"}
(23, 13): {"L F2 L' F U' F"}
--------------------------------------------------
(23, 14), max_length=3: set()
(23, 14), max_length=4: set()
(23, 14), max_length=5: set()
(23, 14), max_length=6: set()
--------------------------------------------------
(23, 15), max_length=3: set()
(23, 15), max_length=4: set()
(23, 15), max_length=5: set()
(23, 15), max_length=6: set()
--------------------------------------------------
"""
