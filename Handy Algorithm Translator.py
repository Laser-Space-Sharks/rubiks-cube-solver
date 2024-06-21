opposites = {'F': 'B',
             'U': 'D',
             'R': 'L',
             'D': 'U',
             'L': 'R',
             'B': 'F'}

ROT_TRAINS = {'F': (('U', 0), ('R', 0), ('D', 2), ('L', 0)),
              'U': (('B', 2), ('R', 1), ('F', 0), ('L', 3)),
              'R': (('U', 3), ('B', 0), ('D', 3), ('F', 0)),
              'D': (('B', 0), ('L', 3), ('F', 2), ('R', 1)),
              'L': (('U', 1), ('F', 0), ('D', 1), ('B', 0)),
              'B': (('U', 2), ('L', 0), ('D', 0), ('R', 0))}

def translate_algorithm_to_FURDLB(algorithm: str):
    '''
    As of the 6/21/24 update, this function handles ALL 18 instructions properly.
    '''
    # THERE EXISTS MOVES WE DO NOT LIKE
    # PARTICULARLY: x, y, z, f, u, r, d, l, b, M, E, S
    # f = B, z
    # u = D, y
    # r = L, x
    # d = U, y'
    # l = R, x'
    # b = F, z'
    # M = L', R, x'
    # E = U, D', y'
    # S = F', B, z
    algorithm = algorithm\
        .replace("'2", "2").replace("2'", "2")\
        .replace("Fw", "f").replace("Uw", 'u').replace("Rw", "r")\
        .replace("Dw", 'd').replace("Lw", 'l').replace("Bw", 'b')\
        .replace("f'", "B' z'").replace("f2", "B2 z2").replace("f", "B z")\
        .replace("u'", "D' y'").replace("u2", "D2 y2").replace("u", "D y")\
        .replace("r'", "L' x'").replace("r2", "L2 x2").replace("r", "L x")\
        .replace("d'", "U' y").replace("d2", "U2 y2").replace("d", "U y'")\
        .replace("l'", "R' x").replace("l2", "R2 x2").replace("l", "R x'")\
        .replace("b'", "F' z").replace("b2", "F2 z2").replace("b", "F z'")\
        .replace("M'", "L R' x").replace("M2", "L2 R2 x2").replace("M", "L' R x'")\
        .replace("E'", "U' D y").replace("E2", "U2 D2 y2").replace("E", "U D' y'")\
        .replace("S'", "F B' z'").replace("S2", "F2 B2 z2").replace("S", "F' B z")\
        .replace("(", "").replace(")", "")
    
    ALG: list[str] = []
    
    #  [front face, rotational shift]
    perspective = ['F', 0]
    AdjFacesToNums = {'U': 0, 'R': 1, 'D': 2, 'L': 3}
    for move1 in algorithm.split():
        if len(move1)==1: root, MODIFIER = move1, ""
        else: root, MODIFIER = move1[:-1], move1[-1]

        if root in {'F', 'U', 'R', 'D', 'L', 'B'}:
            if root == 'F': ALG.append(f"{perspective[0]}{MODIFIER}")
            elif root == 'B': ALG.append(f"{opposites[perspective[0]]}{MODIFIER}")
            else: ALG.append(f"{ROT_TRAINS[perspective[0]][(AdjFacesToNums[root]+perspective[1])%4][0]}{MODIFIER}")
        elif root == 'x':
            if MODIFIER == "'":
                thing = ROT_TRAINS[perspective[0]][(AdjFacesToNums['U']+perspective[1])%4]
                perspective = (thing[0], (perspective[1] + thing[1])%4)
            else:
                for _ in range(1 + (MODIFIER == "2")):
                    thing = ROT_TRAINS[perspective[0]][(AdjFacesToNums['D']+perspective[1])%4]
                    perspective = (thing[0], (perspective[1] + thing[1])%4)
        elif root == 'y':
            if MODIFIER == "'":
                thing = ROT_TRAINS[perspective[0]][(AdjFacesToNums['L']+perspective[1])%4]
                perspective = (thing[0], (perspective[1] + thing[1])%4)
            else:
                for _ in range(1 + (MODIFIER == "2")):
                    thing = ROT_TRAINS[perspective[0]][(AdjFacesToNums['R']+perspective[1])%4]
                    perspective = (thing[0], (perspective[1] + thing[1])%4)
        elif root == 'z':
            if MODIFIER == "'":
                perspective[1] = (perspective[1]+1)%4
            else:
                for _ in range(1 + (MODIFIER == "2")):
                    perspective[1] = (perspective[1]-1)%4
    return ALG
def simplify_FURDLB_MOVES(MOVES) -> list[str]:
    mods_to_nums = {"'": -1,
                    "2": 2}
    nums_to_mods = ["0", "", "2", "'"]
    while True:
        new = []
        modified = False
        i = 0
        while i < len(MOVES):
            if i >= len(MOVES)-1: 
                new.append(MOVES[i])
                i += 1
                continue
            v1, v2 = MOVES[i:i+2]
            if v1[0] == v2[0]:
                modified = True
                sum_of_modifiers = nums_to_mods[(mods_to_nums.get([v1[-1]], 1) + mods_to_nums.get([v2[-1]], 1))%4]
                if sum_of_modifiers != "0": new.append(f"{v1[0]}{sum_of_modifiers}")
                i += 2
            else: 
                new.append(v1)
                i += 1
        MOVES = new[:]
        new = []
        i = 0
        while i < len(MOVES):
            if i >= len(MOVES)-2: 
                new.append(MOVES[i])
                i += 1
                continue
            v1, v2, v3 = MOVES[i:i+3]
            if v1[0] == v3[0] and (v1[0], v2[0]) in {('U', 'D'), ('D', 'U'), ('F', 'B'), ('B', 'F'), ('R', 'L'), ('L', 'R')}:
                modified = True
                sum_of_modifiers = nums_to_mods[(mods_to_nums.get([v1[-1]], 1) + mods_to_nums.get([v3[-1]], 1))%4]
                if sum_of_modifiers != "0": new.append(f"{v1[0]}{sum_of_modifiers}")
                new.append(v2)
                i += 3
            else: 
                new.append(v1)
                i += 1
        MOVES = new[:]
        if not modified: return MOVES

inp_string = input("Input confusing algorithm here: ")
string = ' '.join([j.strip('"') for j in inp_string.strip().split()])
better = ' '.join(simplify_FURDLB_MOVES(translate_algorithm_to_FURDLB(string)))
print(better)
