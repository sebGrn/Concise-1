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
    return len(keys)

def getDensity(_file, column_count, line_count):
    _file = open(_file, "r")
    lines = _file.readlines()
    count = 0

    for line in lines: 
        count += len(line.split(" "))

    _file.close()
    return count / (column_count * line_count)


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