import numpy as np
'''
IMPORTANT NOTICE: THIS GHOSTCUBE DOES NOT USE THE LAST TWO BITS AND IS NON_RELATIVE
'''
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
def update_GhostCube_with_move(GhostCube, move):
                             # U  R  F  L  B  D
    UpCornerTable = np.array([[9, 4, 1, 2, 3, 9],  # U
                              [2, 9, 5, 9, 0, 4],  # R
                              [3, 0, 9, 5, 9, 1],  # F
                              [4, 9, 0, 9, 5, 2],  # L
                              [1, 5, 9, 0, 9, 3],  # B
                              [9, 2, 3, 4, 1, 9]]) # D
                               # U  R  F  L  B  D
    DownCornerTable = np.array([[9, 2, 3, 4, 1, 9],  # U
                                [4, 9, 0, 9, 5, 2],  # R
                                [1, 5, 9, 0, 9, 3],  # F
                                [2, 9, 5, 9, 0, 4],  # L
                                [3, 0, 9, 5, 9, 1],  # B
                                [9, 4, 1, 2, 3, 9]]) # D
    cycles = [[1, 2, 3, 4]
            [2, 0, 4, 5],
            [0, 1, 5, 3],
            [0, 2, 5, 4],
            [1, 0, 3, 5],
            [2, 1, 4, 3]]
    count = {"'": 3, "2": 2}.get(move[-1], 1)
    npind = lambda x, y, z: (x+1, y+1, z+1)
    # Edges
    for x, y, z in ((-1, 0, 1), (1, 0, 1), (0, -1, 1), (0, 1, 1),
                    (-1, -1, 0), (-1, 1, 0), (1, -1, 0), (1, 1, 0),
                    (-1, 0, -1), (1, 0, -1), (0, -1, -1), (0, 1, -1)):
        a0 = GhostCube[npind(x, y, z)]
        b1 = a0>>3
        b2 = a0&7
        if b1 == move[0]: GhostCube[npind(x, y, z)] = (b1<<3)|cycles[move[0]][(cycles[move[0]].index(b2)+count)%4]
        if b2 == move[0]: GhostCube[npind(x, y, z)] = (cycles[move[0]][(cycles[move[0]].index(b1)+count)%4]<<3)|b2
    # Up corners
    for x, y, z in ((-1, -1, 1), (-1, 1, 1), (1, -1, 1), (1, 1, 1)):
        a0 = GhostCube[npind(x, y, z)]
        b1 = a0>>3
        b2 = a0&7
        b3 = UpCornerTable[b1][b2]
        if b1 == move[0]: GhostCube[npind(x, y, z)] = (b1<<3)|cycles[move[0]][(cycles[move[0]].index(b2)+count)%4]
        if b2 == move[0]: GhostCube[npind(x, y, z)] = (cycles[move[0]][(cycles[move[0]].index(b1)+count)%4]<<3)|b2
        if b3 == move[0]: GhostCube[npind(x, y, z)] = (cycles[move[0]][(cycles[move[0]].index(b1)+count)%4]<<3)|cycles[move[0]][(cycles[move[0]].index(b2)+count)%4]
    # Down corners
    for x, y, z in ((-1, -1, -1), (-1, 1, -1), (1, -1, -1), (1, 1, -1)):
        a0 = GhostCube[npind(x, y, z)]
        b1 = a0>>3
        b2 = a0&7
        b3 = DownCornerTable[b1][b2]
        if b1 == move[0]: GhostCube[npind(x, y, z)] = (b1<<3)|cycles[move[0]][(cycles[move[0]].index(b2)+count)%4]
        if b2 == move[0]: GhostCube[npind(x, y, z)] = (cycles[move[0]][(cycles[move[0]].index(b1)+count)%4]<<3)|b2
        if b3 == move[0]: GhostCube[npind(x, y, z)] = (cycles[move[0]][(cycles[move[0]].index(b1)+count)%4]<<3)|cycles[move[0]][(cycles[move[0]].index(b2)+count)%4]

