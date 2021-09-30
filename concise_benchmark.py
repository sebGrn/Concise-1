import sys
import os
import shutil
import subprocess
import argparse

def createCustomConcise(line_count, column_count, delay):
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

# ----------------------------------------------------------------------------------------------------------- #

files = os.listdir("./BenchmarkOutput")
for f in files:
    os.remove("./BenchmarkOutput/" + f)

parser = argparse.ArgumentParser()
parser.add_argument('--delay', default="100000")
args = parser.parse_args()

for _file in os.listdir("./Benchmark"):
    with open("./Benchmark/" + _file, "r") as file :
        line_count = 0

        while 1:
            buffer = file.read(8192*1024)
            if not buffer: 
                break
            line_count += buffer.count('\n')

    column_count = getNumberColumn("./Benchmark/" + _file)

    createCustomConcise(line_count, column_count, args.delay)
        
    '''
    1 : input file
    2 : mandatory
    3 : use mesures
    4 : threshold
    5 : output file
    6 : use parallelism
    7 : constant memory mode
    8 : benchmark mode
    9 : delay
    '''

    threshold = 1.0
    while threshold > 0.5:
        p = subprocess.Popen(["./Concise", 
            "../../Benchmark/" + _file, 
            "True", 
            "False", 
            str(threshold), 
            "../../Benchmark/" + _file, 
            "True",
            "True",
            "True",
            args.delay],
            cwd=str(line_count) + "x" + str(column_count) + "/cmake")
        p.wait()

        p = subprocess.Popen(["./Concise", 
            "../../Benchmark/" + _file, 
            "True", 
            "False", 
            str(threshold), 
            "../../Benchmark/" + _file, 
            "False",
            "True",
            "True",
            args.delay],
            cwd=str(line_count) + "x" + str(column_count) + "/cmake")
        p.wait()

        p = subprocess.Popen(["./Concise", 
            "../../Benchmark/" + _file, 
            "False", 
            "False", 
            str(threshold), 
            "../../Benchmark/" + _file, 
            "True",
            "True",
            "True",
            args.delay],
            cwd=str(line_count) + "x" + str(column_count) + "/cmake")
        p.wait()

        p = subprocess.Popen(["./Concise", 
            "../../Benchmark/" + _file, 
            "False", 
            "False", 
            str(threshold), 
            "../../Benchmark/" + _file, 
            "False",
            "True",
            "True",
            args.delay],
            cwd=str(line_count) + "x" + str(column_count) + "/cmake")
        p.wait()

        threshold -= 0.1
