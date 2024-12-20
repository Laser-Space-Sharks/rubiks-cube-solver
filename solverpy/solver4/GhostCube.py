import numpy as np
faces = "URFLBD"
faces_to_nums = {'U': 0,
                 'R': 1,
                 'F': 2,
                 'L': 3,
                 'B': 4,
                 'D': 5}
def GhostCube_from_cube2ds(cube2ds):
    # x, y, z point F, R, and U, respectively.
    bitmask = lambda a: np.uint8((a[0]<<3)|(a[1]))
    npind1 = lambda x: (x[0]+2, x[1]+2, x[2]+2)
    cube3d = np.zeros(shape=(5, 5, 5), dtype=np.uint8)
    npind2 = lambda x, y, z: (x+1, y+1, z+1)
    GhostCube = np.zeros(shape=(3, 3, 3), dtype=np.uint8)
    def getface(a):
        if a[2] == 2: return 0
        if a[1] == 2: return 1
        if a[0] == 2: return 2
        if a[1] == -2: return 3
        if a[0] == -2: return 4
        if a[2] == -2: return 5
    ''' With the old coordinate system
    for i in (-1, 0, 1):
        for j in (-1, 0, 1):
            cube3d[i+2, 4, j+2] = faces_to_nums[cube2ds['U'][(1-j, i+1)]]
            cube3d[4, j+2, i+2] = faces_to_nums[cube2ds['R'][(1-j, i+1)]]
            cube3d[i+2, j+2, 0] = faces_to_nums[cube2ds['F'][(1-j, i+1)]]
            cube3d[0, i+2, j+2] = faces_to_nums[cube2ds['L'][(1-i, 1-j)]]
            cube3d[i+2, j+2, 4] = faces_to_nums[cube2ds['B'][(1-j, 1-i)]]
            cube3d[i+2, 0, j+2] = faces_to_nums[cube2ds['D'][(1-j, 1-i)]]
    '''''' new coordinate system, but unsimplified
    for i in (-1, 0, 1):
        for j in (-1, 0, 1):
            cube3d[npind1((i, j, 2))] = faces_to_nums[cube2ds['U'][i+1, j+1]]
            cube3d[npind1((i, 2, j))] = faces_to_nums[cube2ds['R'][1-j, 1-i]]
            cube3d[npind1((2, i, j))] = faces_to_nums[cube2ds['F'][1-j, i+1]]
            cube3d[npind1((i, -2, j))] = faces_to_nums[cube2ds['L'][1-j, i+1]]
            cube3d[npind1((-2, i, j))] = faces_to_nums[cube2ds['B'][1-j, 1-i]]
            cube3d[npind1((i, j, -2))] = faces_to_nums[cube2ds['D'][i+1, 1-j]]
    ''' # New coordinate system, and simplified
    for i in range(3):
        for j in range(3):
            cube3d[i+1, j+1, 4] = faces_to_nums[cube2ds['U'][i, j]]
            cube3d[i+1, 4, j+1] = faces_to_nums[cube2ds['R'][-j, -i]]
            cube3d[4, i+1, j+1] = faces_to_nums[cube2ds['F'][-j, i]]
            cube3d[i+1, 0, j+1] = faces_to_nums[cube2ds['L'][-j, i]]
            cube3d[0, i+1, j+1] = faces_to_nums[cube2ds['B'][-j, -i]]
            cube3d[i+1, j+1, 0] = faces_to_nums[cube2ds['D'][i, -j]]
    # corners
    for x, y, z in ((-1, -1, -1), (-1, -1, 1), (-1, 1, -1), (-1, 1, 1), (1, -1, -1), (1, -1, 1), (1, 1, -1), (1, 1, 1)):
        corner1 = ((2*x, y, z), (x, 2*y, z), (x, y, 2*z))
        corner2 = list(map((lambda a: cube3d[npind1(a)]), corner1))
        corner3 = list(map((lambda a: getface(a)), corner1))

        if set(corner2) == {1, 2, 0}: GhostCube[npind2(1, 1, 1)] = bitmask((corner3[corner2.index(2)], corner3[corner2.index(1)]))
        if set(corner2) == {2, 3, 0}: GhostCube[npind2(1, -1, 1)] = bitmask((corner3[corner2.index(3)], corner3[corner2.index(2)]))
        if set(corner2) == {3, 4, 0}: GhostCube[npind2(-1, -1, 1)] = bitmask((corner3[corner2.index(4)], corner3[corner2.index(3)]))
        if set(corner2) == {4, 1, 0}: GhostCube[npind2(-1, 1, 1)] = bitmask((corner3[corner2.index(1)], corner3[corner2.index(4)]))

        if set(corner2) == {1, 2, 5}: GhostCube[npind2(1, 1, -1)] = bitmask((corner3[corner2.index(2)], corner3[corner2.index(1)]))
        if set(corner2) == {2, 3, 5}: GhostCube[npind2(1, -1, -1)] = bitmask((corner3[corner2.index(3)], corner3[corner2.index(2)]))
        if set(corner2) == {3, 4, 5}: GhostCube[npind2(-1, -1, -1)] = bitmask((corner3[corner2.index(4)], corner3[corner2.index(3)]))
        if set(corner2) == {4, 1, 5}: GhostCube[npind2(-1, 1, -1)] = bitmask((corner3[corner2.index(1)], corner3[corner2.index(4)]))
    # edges
    for x, y, z in ((-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0),
               (-1, 0, -1), (-1, 0, 1), (1, 0, -1), (1, 0, 1),
               (0, -1, -1), (0, -1, 1), (0, 1, -1), (0, 1, 1)):
        if x==0: edge1 = ((0, 2*y, z), (0, y, 2*z))
        if y==0: edge1 = ((2*x, 0, z), (x, 0, 2*z))
        if z==0: edge1 = ((2*x, y, 0), (x, 2*y, 0))
        edge2 = list(map((lambda a: cube3d[npind1(a)]), edge1))
        edge3 =list(map((lambda a: getface(a)), edge1))

        if set(edge2) == {1, 0}: GhostCube[npind2(0, 1, 1)] = bitmask((edge3[edge2.index(1)], edge3[edge2.index(0)]))
        if set(edge2) == {2, 0}: GhostCube[npind2(1, 0, 1)] = bitmask((edge3[edge2.index(2)], edge3[edge2.index(0)]))
        if set(edge2) == {3, 0}: GhostCube[npind2(0, -1, 1)] = bitmask((edge3[edge2.index(3)], edge3[edge2.index(0)]))
        if set(edge2) == {4, 0}: GhostCube[npind2(-1, 0, 1)] = bitmask((edge3[edge2.index(4)], edge3[edge2.index(0)]))

        if set(edge2) == {1, 2}: GhostCube[npind2(1, 1, 0)] = bitmask((edge3[edge2.index(2)], edge3[edge2.index(1)]))
        if set(edge2) == {2, 3}: GhostCube[npind2(1, -1, 0)] = bitmask((edge3[edge2.index(3)], edge3[edge2.index(2)]))
        if set(edge2) == {3, 4}: GhostCube[npind2(-1, -1, 0)] = bitmask((edge3[edge2.index(4)], edge3[edge2.index(3)]))
        if set(edge2) == {4, 1}: GhostCube[npind2(-1, 1, 0)] = bitmask((edge3[edge2.index(1)], edge3[edge2.index(4)]))

        if set(edge2) == {1, 5}: GhostCube[npind2(0, 1, -1)] = bitmask((edge3[edge2.index(1)], edge3[edge2.index(5)]))
        if set(edge2) == {2, 5}: GhostCube[npind2(1, 0, -1)] = bitmask((edge3[edge2.index(2)], edge3[edge2.index(5)]))
        if set(edge2) == {3, 5}: GhostCube[npind2(0, -1, -1)] = bitmask((edge3[edge2.index(3)], edge3[edge2.index(5)]))
        if set(edge2) == {4, 5}: GhostCube[npind2(-1, 0, -1)] = bitmask((edge3[edge2.index(4)], edge3[edge2.index(5)]))
    return GhostCube
'''
cycles = [[1, 2, 3, 4],
        [2, 0, 4, 5],
        [0, 1, 5, 3],
        [0, 2, 5, 4],
        [1, 0, 3, 5],
        [2, 1, 4, 3]]
next_face = np.ones(shape=(3, 6, 6))*9
for count in (1, 2, 3):
    for move in range(6):
        for face in cycles[move]:
            next_face[count-1][move][face] = cycles[move][(cycles[move].index(face)+count)%4]
print(next_face)
'''
next_faces = [[[9, 2, 3, 4, 1, 9],
                [4, 9, 0, 9, 5, 2],
                [1, 5, 9, 0, 9, 3],
                [2, 9, 5, 9, 0, 4],
                [3, 0, 9, 5, 9, 1],
                [9, 4, 1, 2, 3, 9]],

                [[9, 3, 4, 1, 2, 9],
                [5, 9, 4, 9, 2, 0],
                [5, 3, 9, 1, 9, 0],
                [5, 9, 4, 9, 2, 0],
                [5, 3, 9, 1, 9, 0],
                [9, 3, 4, 1, 2, 9]],

                [[9, 4, 1, 2, 3, 9],
                [2, 9, 5, 9, 0, 4],
                [3, 0, 9, 5, 9, 1],
                [4, 9, 0, 9, 5, 2],
                [1, 5, 9, 0, 9, 3],
                [9, 2, 3, 4, 1, 9]]]
def Down_corner_after_moves(moves: list[tuple[int, int]], bitmasked_tuple: np.uint8):
                               # U  R  F  L  B  D
    DownCornerTable = np.array([[9, 2, 3, 4, 1, 9],  # U
                                [4, 9, 0, 9, 5, 2],  # R
                                [1, 5, 9, 0, 9, 3],  # F
                                [2, 9, 5, 9, 0, 4],  # L
                                [3, 0, 9, 5, 9, 1],  # B
                                [9, 4, 1, 2, 3, 9]]) # D
    b1 = bitmasked_tuple>>3
    b2 = bitmasked_tuple&7
    b3 = DownCornerTable[b1][b2]
    for move in moves:
        mv = move[0]
        count = move[1]
        if b1 == mv: 
            b2 = next_faces[count-1][mv][b2]
            b3 = next_faces[count-1][mv][b3]
        if b2 == mv: 
            b1 = next_faces[count-1][mv][b1]
            b3 = next_faces[count-1][mv][b3]
        if b3 == mv: 
            b1 = next_faces[count-1][mv][b1]
            b2 = next_faces[count-1][mv][b2]
    return (b1<<3)|b2
def Up_corner_after_moves(moves: list[tuple[int, int]], bitmasked_tuple: np.uint8):
                             # U  R  F  L  B  D
    UpCornerTable = np.array([[9, 4, 1, 2, 3, 9],  # U
                              [2, 9, 5, 9, 0, 4],  # R
                              [3, 0, 9, 5, 9, 1],  # F
                              [4, 9, 0, 9, 5, 2],  # L
                              [1, 5, 9, 0, 9, 3],  # B
                              [9, 2, 3, 4, 1, 9]]) # D
    b1 = bitmasked_tuple>>3
    b2 = bitmasked_tuple&7
    b3 = UpCornerTable[b1][b2]
    for move in moves:
        mv = move[0]
        count = move[1]
        if b1 == mv: 
            b2 = next_faces[count-1][mv][b2]
            b3 = next_faces[count-1][mv][b3]
        if b2 == mv: 
            b1 = next_faces[count-1][mv][b1]
            b3 = next_faces[count-1][mv][b3]
        if b3 == mv: 
            b1 = next_faces[count-1][mv][b1]
            b2 = next_faces[count-1][mv][b2]
    return (b1<<3)|b2
def Edge_after_moves(moves: list[tuple[int, int]], bitmasked_tuple: np.uint8):
    b1 = bitmasked_tuple>>3
    b2 = bitmasked_tuple&7
    for move in moves:
        mv = move[0]
        count = move[1]
        if b1 == mv: 
            b2 = next_faces[count-1][mv][b2]
        if b2 == mv: 
            b1 = next_faces[count-1][mv][b1]
    return (b1<<3)|b2
