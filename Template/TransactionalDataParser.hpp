#pragma once
#include <map>
#include "DataConverter.hpp"

using std::ifstream;
using std::string;
using std::map;

class Runner;

class TransactionalDataParser
{
public:
    TransactionalDataParser();
    bool parse(Runner* runner, const std::string& inputFilename);
    map<string, int> getMapStringToInt();
    map<int, string> getMapIntToString();
    bool initIntentConcept(Runner* runner);

private:
    map<string, int> dataStringToInt;
    map<int, string> dataIntToString;
};

