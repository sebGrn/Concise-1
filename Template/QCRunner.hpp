#pragma once
#include <string>
#include <fstream>
#include <set>
#include <string>
#include <map>
#include <tuple>
#include <unordered_set>
#include <vector>
#include <utility>
#include <sstream>
#include <iomanip>  // std::setprecision
#include <omp.h>

#include "TransactionalDataParser.hpp"
#include "Logger.hpp"
#include "Bitset_Concept.hpp"

using std::ofstream;
using std::map;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::greater;

class QCRunner
{
    friend class TransactionalDataParser;

public:
    QCRunner(const std::string& _outputFilename, float _threshold, bool useMeasures, bool _useMandatory, bool _useParallelism, bool _constantMemoryMode, bool _benchmarkMode);
    ~QCRunner();

    void runFcaCemb(const std::string& inputFilename);
    void writeStats();

private:
    float threshold;
    bool useMeasures;
    size_t nb_covered_tuples;
    size_t total;
    std::string outputFilename;
    ofstream clockFileStream;

    // The string separating dimensions
    std::string dimensionSeparator;

    // List of concepts to write in output file
    vector<pair<std::bitset<$_LINE_NUMBER>, std::bitset<$_COLUMN_NUMBER>>> mandatoryConcepts;
    vector<pair<std::bitset<$_LINE_NUMBER>, std::bitset<$_COLUMN_NUMBER>>> nonMandatoryConcepts;

    // concept1 has an extent and an intent
    BitsetConcept concept;

    // map data from int to string
    map<string, int> dataStringToInt;
    // map data from string to int
    map<int, string> dataIntToString;

    // Store measures
    map<string, float> measures;
    bool useMandatory;
    std::vector < std::pair<double, std::pair<size_t, size_t >> > size_tuples;
    bool useParallelism;
    bool constantMemoryMode;
    bool benchmarkMode;
    int nbMandatoryConcept = 0;
    int nbNonMandatoryConcept = 0;
    int nb_threads;

    string calculateMeasures(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent);
    void prepareFile(const std::string& inputFilename);

    float calculateMeasureObjectUniformity(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent);
    float calculateMeasureMonocle(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent);
    float calculateMeasureFrequency(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent);
    float calculateMeasureSeparation(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent);

    int lookForMandatoryFcaCemb(size_t i, vector<std::bitset<$_LINE_NUMBER>>& covered_tuples);
    void computeFcaCemb();

    void writeTuples();
    void writeSingleTuple(bool append, bool mandatory, pair<std::bitset<$_LINE_NUMBER>, std::bitset<$_COLUMN_NUMBER>> tuple);
    std::string printIntent(std::bitset<$_COLUMN_NUMBER> db);
    std::string printExtent(std::bitset<$_LINE_NUMBER> db);
    void writeSummary();
    void waitForTask(vector<pair<size_t, size_t>> &max_values_and_indices, int &state, int &best_i, int &max_val, vector<std::bitset<$_LINE_NUMBER>> &covered_tuples);
};
