import sys
import os
import shutil
import subprocess
import argparse

def getNumberColumn(_file):
    _file = open(_file, "r")
    madeOfInt = True
    lines = _file.readlines() 
    keys = set()
    maxInt = 0

    for line in lines: 
        for char in line.split(" "):
            if not(char.isspace()):
                keys.add(char)
                if not(char.isdigit()):
                    madeOfInt = False
                elif(maxInt < int(char)):
                    maxInt = int(char)
    
    _file.close()
    return maxInt

def getDensity(_file, column_count, line_count):
    _file = open(_file, "r")
    lines = _file.readlines()
    count = 0

    for line in lines: 
        count += len(line.split(" ")[:-1])

    _file.close()
    return count / (column_count * line_count)

def getVonNeumannStress(_file, column_count, line_count):
    _file = open(_file, "r")
    lines = _file.readlines()
    count = 0
    matrix = []
    matrix_stress = 0

    for line in lines:
        current_line = [0] * column_count

        for bit in line.split(" ")[:-1]:
            current_line[int(bit) - 1] = 1
        matrix.append(current_line)

    for i in range(line_count):
        for j in range(column_count):
            column_stress = 0                

            for k in range(max([0, i - 1]),  min(line_count - 1, i + 1) + 1):
                column_stress += (int(matrix[i][j]) - int(matrix[k][j])) ** 2

            row_stress = 0
            for l in range(max([0, j - 1]),  min(column_count - 1, j + 1) + 1):
                row_stress += (int(matrix[i][j]) - int(matrix[i][l])) ** 2

            matrix_stress += row_stress + column_stress

    nbCorners = 4
    nbEdges = 0
    nbInsideCells = 0

    if column_count > 2:
        nbEdges += (column_count - 2) * 2

    if line_count > 2:
        nbEdges += (line_count - 2) * 2

    nbInsideCells = (line_count - 2) * (column_count - 2)

    max_stress = nbCorners*2 + nbEdges*3 + nbInsideCells*4

    return matrix_stress / max_stress

def getMooreStress(_file, column_count, line_count):
    _file = open(_file, "r")
    lines = _file.readlines()
    count = 0
    matrix = []
    matrix_stress = 0

    for line in lines:
        current_line = [0] * column_count

        for bit in line.split(" ")[:-1]:
            current_line[int(bit) - 1] = 1
        matrix.append(current_line)

    for i in range(line_count):
        for j in range(column_count):              
            for k in range(max([0, i - 1]),  min(line_count - 1, i + 1) + 1):
                for l in range(max([0, j - 1]),  min(column_count - 1, j + 1) + 1):
                    matrix_stress += (int(matrix[i][j]) - int(matrix[k][l])) ** 2

    nbCorners = 4
    nbEdges = 0
    nbInsideCells = 0

    if column_count > 2:
        nbEdges += (column_count - 2) * 2

    if line_count > 2:
        nbEdges += (line_count - 2) * 2

    nbInsideCells = (line_count - 2) * (column_count - 2)

    max_stress = nbCorners*2 + nbEdges*3 + nbInsideCells*4

    return matrix_stress / max_stress



for _fileName in os.listdir("./Density"):
    _fileName = "Density/" + _fileName
    _file = open(_fileName, "r")
    line_count = 0

    while 1:
        buffer = _file.read(8192*1024)
        if not buffer: 
            break
        line_count += buffer.count('\n')

    _file.close()
    column_count = getNumberColumn(_fileName)

    print("Density " + _fileName + " : " + str(getDensity(_fileName, column_count, line_count)))
    print("Von neumann stress " + _fileName + " : " + str(getVonNeumannStress(_fileName, column_count, line_count)))
    print("Moore stress " + _fileName + " : " + str(getMooreStress(_fileName, column_count, line_count)))
    print()