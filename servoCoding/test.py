keysAdded = dict()
with open("test.txt", "r") as file:
    for i, line in enumerate(file.readlines()):
        l = line.strip()
        a = l[l.find("New Key added!: ")+len("New Key added!: ") : l.find(", weight=")]
        if a in keysAdded:
            print(f"LINE {keysAdded[a]} AND LINE {i}")
            exit()
        else:
            keysAdded[a] = i