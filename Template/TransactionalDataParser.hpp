#pragma once
#include <map>
#include "DataConverter.hpp"

using std::ifstream;
using std::string;
using std::map;

class QCRunner;

class TransactionalDataParser
{
public:
    TransactionalDataParser();
    bool parse(QCRunner* runner, const std::string& inputFilename);
    map<string, int> getMapStringToInt();
    map<int, string> getMapIntToString();
    bool initIntentConcept(QCRunner* runner);

private:
    map<string, int> dataStringToInt;
    map<int, string> dataIntToString;
};

