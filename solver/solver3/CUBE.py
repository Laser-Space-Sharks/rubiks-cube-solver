import numpy as np
faces = "URFLBD"
faces_to_nums = {'U': 0,
                 'R': 1,
                 'F': 2,
                 'L': 3,
                 'B': 4,
                 'D': 5}
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
    # 'N' represents Null, for the color information 
    #    still has to be transferred from cube3d to cube2ds.
                                     # B
    cube2ds =  {'U' : np.array([['N', 'N', 'N'], 
                                ['N', 'U', 'N'],  # R
                                ['N', 'N', 'N']]), 
                                     # F
                                     # U
                'R' : np.array([['N', 'N', 'N'], 
                                ['N', 'R', 'N'],  # B
                                ['N', 'N', 'N']]),
                                     # D
                                     # U
                'F' : np.array([['N', 'N', 'N'], 
                                ['N', 'F', 'N'],  # R
                                ['N', 'N', 'N']]),
                                     # D
                                     # U
                'L' : np.array([['N', 'N', 'N'], 
                                ['N', 'L', 'N'],  # F
                                ['N', 'N', 'N']]),  
                                     # D
                                     # U
                'B' : np.array([['N', 'N', 'N'], 
                                ['N', 'B', 'N'],  # L
                                ['N', 'N', 'N']]),
                                     # D
                                     # B
                'D' : np.array([['N', 'N', 'N'], 
                                ['N', 'D', 'N'],  # L
                                ['N', 'N', 'N']])}
                                     # F
    for i in (-1, 0, 1):
        for j in (-1, 0, 1):
            cube2ds['U'][(1-j, i+1)] = faces[cube3d[i+2, 4, j+2]]
            cube2ds['R'][(1-j, i+1)] = faces[cube3d[4, j+2, i+2]]
            cube2ds['F'][(1-j, i+1)] = faces[cube3d[i+2, j+2, 0]]
            cube2ds['L'][(1-i, 1-j)] = faces[cube3d[0, i+2, j+2]]
            cube2ds['B'][(1-j, 1-i)] = faces[cube3d[i+2, j+2, 4]]
            cube2ds['D'][(1-j, 1-i)] = faces[cube3d[i+2, 0, j+2]]
    return cube2ds
SOLVED_CUBE = cube3d_from_cube2ds({'U': np.array([['U', 'U', 'U'], 
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
def CUBE3D_from_scramble(scramble):
    cube3d = np.copy(SOLVED_CUBE)
    for move in scramble.split():
        update_cube_with_F_move(cube3d, move)
    return cube3d
def printcube(cube3d) -> None:
    cube = cube2ds_from_cube3d(cube3d)
    for key, value in cube.items():
        print(f'{key}: ')
        print(value)
    print('-------------------------------------')
def update_cube_with_F_move(cube3d, F_move: str) -> None:
    Fmove = F_move[0]
    c = {"'": -1, "2": 2}.get(F_move[-1], 1)
                                                    # axes=(axis1, axis2) means that after the rotation, 
                                                    # anything in the positive axis1 direction 
                                                    # will now be in the positive axis2 direction.
    if Fmove == 'U': cube3d[:,3:,:] = np.rot90(cube3d, k=c, axes=(2, 0))[:,3:,:]
    if Fmove == 'R': cube3d[3:,:,:] = np.rot90(cube3d, k=c, axes=(1, 2))[3:,:,:]
    if Fmove == 'F': cube3d[:,:,:2] = np.rot90(cube3d, k=c, axes=(1, 0))[:,:,:2]
    if Fmove == 'L': cube3d[:2,:,:] = np.rot90(cube3d, k=c, axes=(2, 1))[:2,:,:]
    if Fmove == 'B': cube3d[:,:,3:] = np.rot90(cube3d, k=c, axes=(0, 1))[:,:,3:]
    if Fmove == 'D': cube3d[:,:2,:] = np.rot90(cube3d, k=c, axes=(0, 2))[:,:2,:]
