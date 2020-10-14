#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <cassert>
#include <stack>
#include <iostream>
#include <vector>
#include <unistd.h>
#include "sys/times.h"
#include "string.h"
#include <fstream>

using std::ofstream;
using std::string;

class Logger
{
private:
    static std::stack<std::chrono::time_point<std::chrono::system_clock>> timer_stack;

    static bool verbose;
    static bool printIntoFile;

    static string filename;
public:
    static void start(bool _printToFile, bool _verbose, const std::string& outputFilename);
    static void stop();

    static void start_tick();
    static int64_t end_tick(const std::string& msg);
    static void getRAMUsage(double& vm_usage, double& resident_set);
    static void initCPUMesuring();
    static double getCPUUsage();
    static void logRessourcesUsage(bool& endThread, string outputFilename, std::vector<std::string> parameters, int delay);
};

inline void Logger::start(bool _printToFile, bool _verbose, const std::string& outputFilename)
{
    printIntoFile = _printToFile;
    verbose = _verbose;
    filename = outputFilename;
}

inline void Logger::stop()
{
}

inline void Logger::start_tick()
{
    if (verbose || printIntoFile) {
        auto beginTime = std::chrono::system_clock::now();
        timer_stack.push(beginTime);
    }
}

inline int64_t Logger::end_tick(const std::string& msg)
{
    std::cout << msg << std::endl;
    if (verbose || printIntoFile) {
        assert(timer_stack.size() > 0);
        auto beginTime = timer_stack.top();
        timer_stack.pop();
        ofstream clockFileStream(filename, ofstream::app);
        int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
        if (printIntoFile)
            clockFileStream << msg << " in " << duration << " milliseconds" << std::endl;
        if (verbose)
            std::cout << msg << " in " << duration << " milliseconds" << std::endl;
        clockFileStream.close();
        return duration;
    }
    return 0.0;
}
