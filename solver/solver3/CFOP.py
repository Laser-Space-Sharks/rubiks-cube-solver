from ALG_MANIPULATION import simplify_FURDLB_MOVES
from CUBE import int_to_CI, update_cube_with_F_move, faces_to_nums
from PERSPECTIVES import algorithm_from_Fs_perspective
from ALGORITHMS import piece_indexes, FULL_CROSS, FULL_F2L, FULL_OLL, FULL_PLL
import numpy as np
################# C
def get_algs_for_bottom_edges(cube3d) -> list[list[str]]:
    algorithms = []
    for x0, y0, z0 in ((-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0),
                        (-1, 0, -1), (-1, 0, 1), (1, 0, -1), (1, 0, 1),
                        (0, -1, -1), (0, -1, 1), (0, 1, -1), (0, 1, 1)):
        if x0==0: xyzs = ((0, 2*y0, z0), (0, y0, 2*z0))
        if y0==0: xyzs = ((2*x0, 0, z0), (x0, 0, 2*z0))
        if z0==0: xyzs = ((2*x0, y0, 0), (x0, 2*y0, 0))
        for x, y, z in xyzs:
            if cube3d[x+2, y+2, z+2] != faces_to_nums['D']: continue # make sure it is a *bottom* edge
            if y == -2:
                destination = int_to_CI(cube3d[x+2, 1, z+2])
                if destination == (x, -1, z): continue               # make sure it is *wrong*
            elif y == 2: destination = int_to_CI(cube3d[x+2, 3, z+2])
            elif x == 2: destination = int_to_CI(cube3d[3, y+2, z+2])
            elif x == -2: destination = int_to_CI(cube3d[1, y+2, z+2])
            elif z == 2: destination = int_to_CI(cube3d[x+2, y+2, 3])
            elif z == -2: destination = int_to_CI(cube3d[x+2, y+2, 1])
            if destination == (1, -1, 0): algorithms.append(algorithm_from_Fs_perspective('R', FULL_CROSS[piece_indexes[(z+2, y+2, -x+2)]]))
            elif destination == (0, -1, -1): algorithms.append(algorithm_from_Fs_perspective('F', FULL_CROSS[piece_indexes[(x+2, y+2, z+2)]]))
            elif destination == (-1, -1, 0): algorithms.append(algorithm_from_Fs_perspective('L', FULL_CROSS[piece_indexes[(-z+2, y+2, x+2)]]))
            elif destination == (0, -1, 1): algorithms.append(algorithm_from_Fs_perspective('B', FULL_CROSS[piece_indexes[(-x+2, y+2, -z+2)]]))
    return algorithms
################# F
def get_algs_for_f2l_pairs(cube3d) -> list[list[str]]:
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
def get_OLL_algorithm(cube3d) -> list[str]:
    top_layer_num = 0
    for x, y, z in ((2, 1, 0), (0, 1, -2), (-2, 1, 0), (2, 1, 1),(2, 1, -1),
                    (1, 1, -2),(-1, 1, -2), (-2, 1, -1),(1, 1, 2)):
        top_layer_num = (top_layer_num*2)|(cube3d[x+2, y+2, z+2]==0)
    return FULL_OLL[top_layer_num].split()
################# P
def get_PLL_algorithm(cube3d) -> list[str]:
    return FULL_PLL[(int(cube3d[4, 3, 3])-1)*256
                  + (int(cube3d[3, 3, 0])-1)*64
                  + (int(cube3d[4, 3, 2])-1)*16
                  + (int(cube3d[2, 3, 0])-1)*4
                  + (int(cube3d[0, 3, 2])-1)].split()
########################### Recursive Search for optimal CFOP
def OP(F_moves, cube3d, shortest_length) -> list[str] | None: # At the end of Recursive_F, OP is called, thereby completing the recursive CFOP
    cube3dnew = np.copy(cube3d)
    algorithmOLL = get_OLL_algorithm(cube3dnew)
    if len(simplify_FURDLB_MOVES(F_moves + algorithmOLL)) >= shortest_length[0]: return
    for move in algorithmOLL: update_cube_with_F_move(cube3dnew, move)
    SOLVE_SEQUENCE = simplify_FURDLB_MOVES(F_moves + algorithmOLL + get_PLL_algorithm(cube3dnew))
    if len(SOLVE_SEQUENCE) < shortest_length[0]: shortest_length[0] = len(SOLVE_SEQUENCE)
    return SOLVE_SEQUENCE
def Recursive_F(F_moves, cube3d, shortest_length) -> list[str] | None: # At the end of Recursive_C, Recursive_F is called
    algorithms = get_algs_for_f2l_pairs(cube3d)
    best = None
    if not algorithms: 
        #print(simplify_FURDLB_MOVES(F_moves))
        return OP(F_moves, cube3d, shortest_length)
    for alg in algorithms:
        if len(simplify_FURDLB_MOVES(F_moves + alg)) >= shortest_length[0]: continue
        cube3dnew = np.copy(cube3d)
        for move in alg: update_cube_with_F_move(cube3dnew, move)
        candidate = Recursive_F(F_moves + alg, cube3dnew, shortest_length)
        if candidate is None: continue
        if best is None or len(candidate) < len(best):
            best = candidate
    return best
def Recursive_C(F_moves, cube3d, shortest_length) -> list[str]:
    algorithms = get_algs_for_bottom_edges(cube3d)
    best = None
    if not algorithms: 
        #print(simplify_FURDLB_MOVES(F_moves))
        return Recursive_F(F_moves, cube3d, shortest_length)
    for alg in algorithms:
        if len(simplify_FURDLB_MOVES(F_moves + alg)) >= shortest_length[0]: continue
        cube3dnew = np.copy(cube3d)
        for move in alg: update_cube_with_F_move(cube3dnew, move)
        candidate = Recursive_C(F_moves + alg, cube3dnew, shortest_length)
        if best is None or (candidate is not None and len(candidate) < len(best)):
            best = candidate
    return best

def full_recursive_CFOP(cube3d):
    return Recursive_C([], cube3d, [85])
