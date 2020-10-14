#include "Logger.hpp"

bool Logger::verbose = false;
bool Logger::printIntoFile = true;
std::string Logger::filename;
std::stack<std::chrono::time_point<std::chrono::system_clock>> Logger::timer_stack = std::stack<std::chrono::time_point<std::chrono::system_clock>>();
static clock_t lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;

void Logger::getRAMUsage(double& vm_usage, double& resident_set){ //Note: this value is in KB!
    vm_usage = 0.0;
    resident_set = 0.0;
    std::ifstream stat_stream("/proc/self/stat",std::ios_base::in); //get info from proc

    //create some variables to get info
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;
    unsigned long vsize;
    long rss;
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                >> utime >> stime >> cutime >> cstime >> priority >> nice
                >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care
    stat_stream.close();
    vm_usage = vsize / 1024.0;
    resident_set = rss * 4;
}

void Logger::initCPUMesuring() {
    FILE* file;
    struct tms timeSample;
    char line[128];

    int lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    file = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;
    while(fgets(line, 128, file) != NULL){
        if (strncmp(line, "processor", 9) == 0) numProcessors++;
    }
    fclose(file);
}

double Logger::getCPUUsage() {
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
        timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        percent = (timeSample.tms_stime - lastSysCPU) +
                  (timeSample.tms_utime - lastUserCPU);
        percent /= (now - lastCPU);
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    return percent;
}

void Logger::logRessourcesUsage(bool &endThread, string outputFilename, std::vector<std::string> parameters, int delay) {
    auto start = std::chrono::system_clock::now();
    double vm, rss, cpu;
    initCPUMesuring();
    int cpt = 0;

    std::ofstream outputFileRessourceUsage;
    outputFileRessourceUsage.open(outputFilename + ".stats", ofstream::app);

    outputFileRessourceUsage << std::endl;
    outputFileRessourceUsage << "___________________________________________"<< std::endl;
    outputFileRessourceUsage << "Mandatory: " << parameters.at(0) << " Parallelism: " << parameters.at(1) << " Threshold: " << parameters.at(2) << std::endl;
    outputFileRessourceUsage << std::endl;
    outputFileRessourceUsage << "TIME VM RSS CPU" << std::endl;

    while(!endThread) {
        getRAMUsage(vm, rss);
        cpu = getCPUUsage();
        long double time = (delay * cpt) / 1000;

        if(cpu == -1)
            outputFileRessourceUsage << time << " " << vm << " " << rss << " -" << std::endl;
        else
            outputFileRessourceUsage << time << " " << vm << " " << rss << " " << cpu << std::endl;
        cpt++;

        usleep(delay);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    outputFileRessourceUsage << "Elapsed time: " << elapsed.count() << "s" << std::endl;
    outputFileRessourceUsage << std::endl;

    outputFileRessourceUsage.close();
}


