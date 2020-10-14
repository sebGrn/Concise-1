import sys
import os
import shutil
import subprocess
import argparse

def createCustomQualityCover(line_count, column_count, isBinary, mandatory, delay = 100000):
    if not os.path.exists(str(line_count) + "x" + str(column_count)):
        os.makedirs(str(line_count) + "x" + str(column_count))
        os.makedirs(str(line_count) + "x" + str(column_count) + "/cmake")

        for _file in os.listdir("./Template"):
            if(".cpp" in _file or ".hpp" in _file or "CMakeLists.txt" in _file):
                sourceFile = os.path.join("./Template", _file)
                destinationFile = os.path.join(str(line_count) + "x" + str(column_count), _file)
                if os.path.isdir(sourceFile):
                    shutil.copytree(sourceFile, destinationFile, False, None)
                else:
                    shutil.copy2(sourceFile, destinationFile)

                # Read in the file
                with open(destinationFile, 'r') as file :
                    filedata = file.read()

                # Replace the target string
                filedata = filedata.replace('$_COLUMN_NUMBER', str(column_count))
                filedata = filedata.replace('$_LINE_NUMBER', str(line_count))

                # Write the file out again
                with open(destinationFile, 'w') as file:
                    file.write(filedata)

        p = subprocess.Popen(["cmake", "../", "-DCMAKE_BUILD_TYPE=Release"], cwd=str(line_count) + "x" + str(column_count) + "/cmake")
        p.wait()

        p = subprocess.Popen(["make"], cwd=str(line_count) + "x" + str(column_count) + "/cmake")
        p.wait()
        

        
    else:
        print("Version déjà compilée")

def str2bool(v):
    if isinstance(v, bool):
       return v
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Valeur booléenne attendue')

def createTransactionalFile(_file):
    _file = open(_file, "r")
    lines = _file.readlines()
    transactionalMatrice = []

    for line in lines:
        cpt = 1
        newLine = []
        for char in line.split(" "):
            if char == "1":
                newLine.append(cpt)
            cpt += 1
        transactionalMatrice.append(newLine)
    
    _file.close()
    
    with open('temp_trans.data', 'w') as f:
        for line in transactionalMatrice:
            for column in line:
                f.write("%s " % column)
            f.write("\n")

def getNumberColumn(isBinary, _file):
    if(isBinary):
       createTransactionalFile(_file)
       return getNumberColumn(False, "temp_trans.data")

    else:
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

parser = argparse.ArgumentParser()
parser.add_argument('--inputFile', required=True)
parser.add_argument('--mandatory', default="false") 
parser.add_argument('--mesures', default="false") 
parser.add_argument('--threshold', default="1.") 
parser.add_argument('--parallelism', default="false") 
parser.add_argument('--binary', default="false") 
parser.add_argument('--constantMemory', default="true") 
parser.add_argument('--delay', default="100000")
args = parser.parse_args()

_file = open(args.inputFile, "r")
line_count = 0

while 1:
    buffer = _file.read(8192*1024)
    if not buffer: 
        break
    line_count += buffer.count('\n')

_file.close()
column_count = getNumberColumn(str2bool(args.binary), args.inputFile)

createCustomQualityCover(line_count, column_count, str2bool(args.binary), str2bool(args.mandatory))
    
'''
1 : input file
2 : mandatory
3 : use mesures
4 : threshold
5 : output file
6 : use parallelism
7 : constant memory mode
8 : benchmarkmode
9 : delay
'''
p = subprocess.Popen(["./QualityCover", 
    "../../temp_trans.data" if str2bool(args.binary) else "../../" + args.inputFile, 
    str(str2bool(args.mandatory)), 
    str(str2bool(args.mesures)), 
    args.threshold, 
    "../../" + args.inputFile, 
    str(str2bool(args.parallelism)),
    str(str2bool(args.constantMemory)),
    "False",
    args.delay],
    cwd=str(line_count) + "x" + str(column_count) + "/cmake")
p.wait()
