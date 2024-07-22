from CUBE import CUBE3D_from_scramble, printcube, update_cube_with_F_move, SOLVED_CUBE
from CFOP import full_recursive_CFOP
import numpy as np
import time

if __name__ == '__main__':
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
        CUBE3D = CUBE3D_from_scramble(scramble)
        #                     max cross + max f2l + max oll + max pll = 85 moves
        F_MOVES = full_recursive_CFOP(CUBE3D)
        for move in F_MOVES: update_cube_with_F_move(CUBE3D, move)
        if not np.array_equal(CUBE3D, SOLVED_CUBE):
            print("AAAAAAAAAAAAAAAAAAAAAAAAAA")
            printcube(CUBE3D)
        print(f'scramble: {scramble}')
        print(f"CFOP solution: {' '.join(F_MOVES)}")
        print(f'move count: {len(F_MOVES)}')
        move_counts.append(len(F_MOVES))
        t = time.perf_counter() - start
        print(f"{t} s")

    print(f'average move count: {sum(move_counts)/len(move_counts)}')
