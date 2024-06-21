opposites = {'F': 'B',
             'U': 'D',
             'R': 'L',
             'D': 'U',
             'L': 'R',
             'B': 'F'}

ROT_TRAINS = {'F': [('U', 0), ('R', 0), ('D', 2), ('L', 0)],
              'U': [('B', 2), ('R', 1), ('F', 0), ('L', 3)],
              'R': [('U', 3), ('B', 0), ('D', 3), ('F', 0)],
              'D': [('B', 0), ('L', 3), ('F', 2), ('R', 1)],
              'L': [('U', 1), ('F', 0), ('D', 1), ('B', 0)],
              'B': [('U', 2), ('L', 0), ('D', 0), ('R', 0)]}

def translate_algorithm_to_FURDLB(algorithm: str):
    ALG1: list[str] = [(i.strip('()'))[0:2] for i in algorithm.split()]
    ALG2 = []

    #  [front face, rotational shift]
    perspective = ['F', 0]
    ADJ_TRAIN = ['U', 'R', 'D', 'L']

    # THERE EXISTS MOVES WE DO NOT LIKE
    # PARTICULARLY: y, y', y2, r, r', l, l', u, u', f, f'
    # the f move is equivalent to B, and then making a relative perspective change of ('F', 'L')
    # the u move is equivalent to D, and then making a relative perspective change of ('R', 'U')
    # the l move is equivalent to R, and then making a relative perspective change of ('U', 'B')
    # the r move is equivalent to L, and then making a relative perspective change of ('D', 'F')

    for move1 in ALG1:
        if move1[0] in {'F', 'U', 'R', 'D', 'L', 'B'}:
            if len(move1)==1: MODIFIER = ""
            else: MODIFIER = move1[1]

            if move1[0] == 'F': ALG2.append(f"{perspective[0]}{MODIFIER}")
            elif move1[0] == 'B': ALG2.append(f"{opposites[perspective[0]]}{MODIFIER}")
            else:
                ALG2.append(f"{ROT_TRAINS[perspective[0]][(ADJ_TRAIN.index(move1[0])+perspective[1])%4][0]}{MODIFIER}")
        elif move1[0] == 'y':
            if move1 == "y'":
                thing = ROT_TRAINS[perspective[0]][(3+perspective[1])%4]
                perspective[0] = thing[0]
                perspective[1] = (perspective[1] + thing[1])%4
            else:
                thing = ROT_TRAINS[perspective[0]][(1+perspective[1])%4]
                perspective[0] = thing[0]
                perspective[1] = (perspective[1] + thing[1])%4
                if move1[-1]=='2':
                    thing = ROT_TRAINS[perspective[0]][(1+perspective[1])%4]
                    perspective[0] = thing[0]
                    perspective[1] = (perspective[1] + thing[1])%4
        elif move1=='f':
            ALG2.append(opposites[perspective[0]])
            perspective[1] = (perspective[1]-1)%4
        elif move1=="f'":
            ALG2.append(f"{opposites[perspective[0]]}'")
            perspective[1] = (perspective[1]+1)%4
        elif move1=='u':
            ALG2.append(ROT_TRAINS[perspective[0]][(2+perspective[1])%4][0])
            thing = ROT_TRAINS[perspective[0]][(1+perspective[1])%4]
            perspective[0] = thing[0]
            perspective[1] = (perspective[1] + thing[1])%4
        elif move1=="u'":
            ALG2.append(f"{ROT_TRAINS[perspective[0]][(2+perspective[1])%4][0]}'")
            thing = ROT_TRAINS[perspective[0]][(3+perspective[1])%4]
            perspective[0] = thing[0]
            perspective[1] = (perspective[1] + thing[1])%4
        elif move1=='l':
            ALG2.append(ROT_TRAINS[perspective[0]][(1+perspective[1])%4][0])
            thing = ROT_TRAINS[perspective[0]][perspective[1]]
            perspective[0] = thing[0]
            perspective[1] = (perspective[1] + thing[1])%4
        elif move1=="l'":
            ALG2.append(f"{ROT_TRAINS[perspective[0]][(2+perspective[1])%4][0]}'")
            thing = ROT_TRAINS[perspective[0]][(2+perspective[1])%4]
            perspective[0] = thing[0]
            perspective[1] = (perspective[1] + thing[1])%4
        elif move1=='r':
            ALG2.append(ROT_TRAINS[perspective[0]][(3+perspective[1])%4][0])
            thing = ROT_TRAINS[perspective[0]][(2+perspective[1])%4]
            perspective[0] = thing[0]
            perspective[1] = (perspective[1] + thing[1])%4
        elif move1=="r'":
            ALG2.append(f"{ROT_TRAINS[perspective[0]][(3+perspective[1])%4][0]}'")
            thing = ROT_TRAINS[perspective[0]][perspective[1]]
            perspective[0] = thing[0]
            perspective[1] = (perspective[1] + thing[1])%4
    return ALG2
def simplify_F_MOVES(F_MOVES) -> list[str]:
    while True:
        new = []
        modified = False
        i = 0
        while i < len(F_MOVES):
            if i >= len(F_MOVES)-1: 
                new.append(F_MOVES[i])
                i += 1
                continue
            v1 = F_MOVES[i]
            v2 = F_MOVES[i+1]
            if v1[0] == v2[0]:
                modified = True
                count = 0
                if v1[-1] == "'": count -= 1
                if v1[-1] == "2": count += 2
                if v2[-1] == "'": count -= 1
                if v2[-1] == "2": count += 2
                if count == -2: new.append(f"{v1[0]}2")
                if count == -1: new.append(f"{v1[0]}'")
                if count == 1: new.append(f"{v1[0]}")
                if count == 2: new.append(f"{v1[0]}2")
                if count == 3: new.append(f"{v1[0]}'")
                i += 2
            else: 
                new.append(v1)
                i += 1
        F_MOVES = new[:]
        new = []
        i = 0
        while i < len(F_MOVES):
            if i >= len(F_MOVES)-2: 
                new.append(F_MOVES[i])
                i += 1
                continue
            v1 = F_MOVES[i]
            v2 = F_MOVES[i+1]
            v3 = F_MOVES[i+2]
            if v1[0] == v3[0] and (v1[0], v2[0]) in {('U', 'D'), ('D', 'U'), ('F', 'B'), ('B', 'F'), ('R', 'L'), ('L', 'R')}:
                modified = True
                count = 0
                if v1[-1] == "'": count -= 1
                if v1[-1] == "2": count += 2
                if v3[-1] == "'": count -= 1
                if v3[-1] == "2": count += 2
                if count == -2: new.append(f"{v1[0]}2")
                if count == -1: new.append(f"{v1[0]}'")
                if count == 1: new.append(f"{v1[0]}")
                if count == 2: new.append(f"{v1[0]}2")
                if count == 3: new.append(f"{v1[0]}'")
                new.append(v2)
                i += 3
            else: 
                new.append(v1)
                i += 1
        F_MOVES = new[:]
        if not modified: return F_MOVES

inp_string = input("Input confusing algorithm here: ")
string = ' '.join([j.strip('"') for j in inp_string.strip().split()])
better = ' '.join(simplify_F_MOVES(translate_algorithm_to_FURDLB(string)))
print('"'+better+'"')
