import sys
import random

def createTransactionalFile(binaryMatrice):
    transactionalMatrice = []

    for line in binaryMatrice:
        cpt = 1
        newLine = []
        for bit in line:
            if bit:
                newLine.append(cpt)
            cpt += 1
        transactionalMatrice.append(newLine)
        
    with open('trans' + str(len(binaryMatrice)) + 'x' + str(len(binaryMatrice[0])) + '.data', 'w') as f:
        for line in transactionalMatrice:
            for column in line:
                f.write("%s " % column)
            f.write("\n")

if len(sys.argv) != 3:
    print("Veuillez spécifier un nombre de colonnes et un nombre de lignes")
    print("Exemple python3 generate_transactional_file.py 50 50")
    sys.exit()

binaryMatrice = []

for i in range(0, int(sys.argv[1])):
    newColumn = []
    containsAPositiveBit = False

    for j in range(0, int(sys.argv[2])):
        newBit = random.randint(0, 1)
        newColumn.append(newBit)

        if newBit:
            containsAPositiveBit = True
    
    if not(containsAPositiveBit):
        index = random.randint(0, int(sys.argv[2]))
        newColumn[index] = 1
    
    for j in range(0, int(sys.argv[2])):
        if i == 0:
            binaryMatrice.append([])
        binaryMatrice[j].append(newColumn[j])

createTransactionalFile(binaryMatrice)
