import numpy as np
                                    # U  R  F  L  B  D
move_from_Fs_perspective = np.array([[4, 1, 0, 3, 5, 2],  # U
                                     [0, 4, 1, 2, 3, 5],  # R
                                     [0, 1, 2, 3, 4, 5],  # F
                                     [0, 2, 3, 4, 1, 5],  # L
                                     [0, 3, 4, 1, 2, 5],  # B
                                     [4, 3, 5, 1, 0, 2]]) # D
def algorithm_from_Fs_perspective(face: str, algorithm: str) -> list[str]:
    faces = "URFLBD"
    faces_to_nums = {'U': 0,
                    'R': 1,
                    'F': 2,
                    'L': 3,
                    'B': 4,
                    'D': 5}
    return [                                                                      # reapply the modifer (ie. "'", "2")
        f'{faces[move_from_Fs_perspective[(faces_to_nums[face], faces_to_nums[move[0]])]]}{(move[-1]*(len(move)==2))}'
        for move in algorithm.split()
    ]
