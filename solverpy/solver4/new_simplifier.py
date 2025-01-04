def new_simplify(alg1: list[tuple[int, int]], alg2: list[tuple[int, int]]) -> list[list[int, int]]:
    A1, A2 = [[i[0], i[1]] for i in alg1], [[i[0], i[1]] for i in alg2]
    opp = [5, 3, 4, 1, 2, 0]
    l1, l2 = len(A1)-1, 0
    while l1 >= 0 and l2 <= len(alg2)-1:
        if (l1 > 0) and (A1[l1-1][0] == A2[l2][0]) and (A1[l1][0] == opp[A2[l2][0]]):
            count = (A1[l1-1][1]+A2[l2][1])&3
            if count == 0: 
                A1[l1-1] = A1[l1]
                l1 -= 1
                l2 += 1
            else:
                A1[l1-1][1] = count
                l2 += 1
        elif A1[l1][0] == A2[l2][0]: 
            count = (A1[l1][1]+A2[l2][1])%4
            if count == 0: 
                l1 -= 1
                l2 += 1
            else:
                A1[l1][1] = count
                l2 += 1
        elif (l2 < len(alg2)-1) and (A1[l1][0] == A2[l2+1][0]) and (A1[l1][0] == opp[A2[l2][0]]):
            count = (A1[l1][1]+A2[l2+1][1])&3
            if count == 0: 
                A2[l2+1] = A2[l2]
                l1 -= 1
                l2 += 1
            else:
                A2[l2+1][1] = count
                l1 -= 1
        else: break
    #print(f"A1: {A1} | l1: {l1}")
    #print(f"A2: {A2} | l2: {l2}")
    a = []
    for i in range(0, l1+1):
        a.append(A1[i])
    for i in range(l2, len(alg2)):
        a.append(A2[i])
    return a
