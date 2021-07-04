import sys
import os
import shutil
import subprocess
import argparse

def getMatrix(_file):
    _file = open(_file, "r")
    lines = _file.readlines()
    matrix = []

    for line in lines:
        current_line = []

        for bit in line.split(" "):
            current_line.append(int(bit))
        matrix.append(current_line)

    return matrix

def getConcepts(_file, suffix, mandatory):
    _file = open(_file, "r")
    lines = _file.readlines()
    concepts = []

    for line in lines:
        if not("#" in line):
            current_extent = []

            extent = line.split(";")[0]

            for bit in extent.split(","):
                current_extent.append(int(bit))

            current_intent = []

            intent = line.split(";")[1]

            if "Mandatory" in intent:
                mandatory.append(True)
            else:
                mandatory.append(False)

            for bit in intent.split(","):
                bit = bit.replace(" Mandatory", "")
                current_intent.append(int(bit))
            
            concepts.append([current_extent, current_intent])
        else:
            suffix += line

    return concepts, suffix, mandatory

def calculateMeasureObjectUniformity(extent, intent, inputConcept):
    _sum = 0
    a = len(extent)
    axb = a * len(intent)

    for val in extent:
        _sum += axb / (a * len(inputConcept[val - 1]))

    return _sum / len(extent)

def calculateMeasureMonocle(extent, intent, outputConcepts):
    sumA = 0
    sumB = 0

    for val in extent:
        for concept in outputConcepts:
            if not(val in concept[0]):
                sumA += 1

    for val in intent:
        for concept in outputConcepts:
            if not(val in concept[1]):
                sumB += 1

    a = len(extent) + sumA
    b = len(intent) + sumB

    return a * b

def calculateMeasureFrequency(extent, inputConcept):
    extentSize = len(inputConcept)

    return len(extent) / extentSize

def calculateMeasureSeparation(extent, intent, inputConcept):
    axb = len(extent) * len(intent)
    sumg = 0
    sumb = 0

    for val in extent:
        sumg += len(inputConcept[val - 1])

    for val in intent:
        for concept in inputConcept:
            if val in concept:
                sumb += 1

    return axb / (sumg + sumb - axb)

parser = argparse.ArgumentParser()
parser.add_argument('--inputFile', required=True)
parser.add_argument('--conceptsFile', required=True)
args = parser.parse_args()
suffix = ""
mandatory = []

inputConcept = getMatrix(args.inputFile)
outputConcepts, suffix, mandatory = getConcepts(args.conceptsFile, suffix, mandatory)

metrics = []
for concept in outputConcepts:
    extent = concept[0]
    intent = concept[1]

    objectUniformity = calculateMeasureObjectUniformity(extent, intent, inputConcept)
    #monocle = calculateMeasureMonocle(extent, intent, outputConcepts)
    frequency = calculateMeasureFrequency(extent, inputConcept)
    separation = calculateMeasureSeparation(extent, intent, inputConcept)

    metrics.append([objectUniformity, frequency, separation])

metricsName = ["Object Uniformity", "Frequency", "Separation"]
_outputFile = ""
conceptCpt = 0
for concept in outputConcepts:
    _outputFile += ','.join(str(e) for e in concept[0])
    _outputFile += " ; "
    _outputFile += ','.join(str(e) for e in concept[1])

    if mandatory[conceptCpt]:
         _outputFile += " Mandatory"

    _outputFile += "#"
    for i in range(len(metricsName)):
        _outputFile += metricsName[i]
        _outputFile += "="
        _outputFile += str(metrics[conceptCpt][i])
        _outputFile += "; "

    _outputFile = _outputFile[:-2]

    _outputFile += "\n"

    conceptCpt += 1

_outputFile += suffix

with open(args.conceptsFile + "_with_metrics.txt", 'w') as f:
    f.write(_outputFile)

print(_outputFile)