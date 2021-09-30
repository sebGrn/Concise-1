//
// Created by axel on 23/07/2020.
//

#include <iostream>
#include "Runner.hpp"
#include <thread>
#include <regex>
#include <sstream>
#include <string>

int main(int argc, char ** argv) {
    bool useMandatory = false;
    bool useMesures = false;
    bool useParallelism = false;
    bool useConstantMemoryMode = false;
    bool useBenchmarkMode = false;
    std::string mandatory(argv[2]);
    std::string mesures(argv[3]);
    std::string parallelism(argv[6]);
    std::string constantMemoryMode(argv[7]);
    std::string benchmarkMode(argv[8]);


    if(mandatory.find("True") != string::npos)
        useMandatory = true;
    if(mesures.find("True") != string::npos)
        useMesures = true;
    if(parallelism.find("True") != string::npos)
        useParallelism = true;
    if(constantMemoryMode.find("True") != string::npos)
        useConstantMemoryMode = true;
    if(benchmarkMode.find("True") != string::npos)
        useBenchmarkMode = true;

    bool endThread = false;

    std::string outputFileName = argv[5];
    if(useBenchmarkMode){
       std::stringstream result;
       std::regex e ("../../Benchmark/");
       std::regex_replace(std::ostream_iterator<char>(result), outputFileName.begin(), outputFileName.end(), e, "../../BenchmarkOutput/");
       outputFileName = result.str();
       std::cout << outputFileName + "_para=" + parallelism + "_mandatory=" + mandatory + "_threshold=" + std::string(argv[4]) << std::endl;
    }

    std::vector<std::string> parameters = {mandatory, parallelism, std::string(argv[4])};

    std::thread displayRAMThread(&Logger::logRessourcesUsage, std::ref(endThread), outputFileName, parameters, atoi(argv[9]));

    Runner runner(outputFileName, atof(argv[4]), useMesures, useMandatory, useParallelism, useConstantMemoryMode, useBenchmarkMode);
    runner.runFcaCemb(argv[1]);


    endThread = true;
    displayRAMThread.join();

    runner.writeStats();

    return 0;
}


