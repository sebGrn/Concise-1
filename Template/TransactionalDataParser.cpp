#include <map>
#include <string>
#include <vector>
#include "Runner.hpp"
#include "TransactionalDataParser.hpp"

TransactionalDataParser::TransactionalDataParser()
{
}

bool TransactionalDataParser::parse(Runner* runner, const std::string& inputFilename)
{
    Logger::start_tick();

    std::cout << "Parsing transactional file " << inputFilename << std::endl;
    string line;

    ifstream inputFile(inputFilename, ifstream::in);

    if (inputFile.fail()) {
        std::cerr << "Couldn't load file '" << inputFilename << "'" << std::endl;
        exit(1);
    }
    this->dataStringToInt = map<string, int>();
    this->dataIntToString = map<int, string>();

    // first extract a matrix of indexes from transactional data
    unsigned int indexMax = 0;
    std::vector<std::vector<unsigned int>> vecOfInt = DataConverter::extractIndexes2(inputFilename, indexMax, this->dataStringToInt,
                                        this->dataIntToString);
    int cptLine = 0;
    // secondly loop on matrix of indexes
    for_each(vecOfInt.begin(), vecOfInt.end(), [&](const std::vector<unsigned int>& line) {
        // create a bitset
        std::bitset<$_COLUMN_NUMBER> line_bitset;
        for (unsigned int i = 0; i < line.size(); i++) {
            // indexes should start at 1 in transactional datas
            assert(line[i] >= 1);
            unsigned int column = line[i] - 1;
            assert(column < line_bitset.size());
            line_bitset[column] = true;
        }

        //  init extent from concept 1
        {
            if (runner->concept.extent.size() == 0)
                runner->concept.extent = vector<std::bitset<$_LINE_NUMBER>>($_COLUMN_NUMBER);
            // take bit from bitset and add it as element (with dynamic_bitset)
            for (size_t i = 0; i < line_bitset.size(); i++) {
                assert(i < runner->concept.extent.size());
                runner->concept.extent[i][cptLine] = line_bitset[i];
            }
        }

        //  init extent from concept 2
        {
            if (runner->concept.extentNotReversed.size() == 0)
                runner->concept.extentNotReversed = vector<std::bitset<$_COLUMN_NUMBER>>();
            runner->concept.extentNotReversed.push_back(line_bitset);

            // init support
            runner->concept.support.push_back(line_bitset.count());
        }
        cptLine++;
    });
    Logger::end_tick("Parsing");
    return true;
}

map<string, int> TransactionalDataParser::getMapStringToInt()
{
    return this->dataStringToInt;
}

map<int, string> TransactionalDataParser::getMapIntToString()
{
    return this->dataIntToString;
}

bool TransactionalDataParser::initIntentConcept(Runner* runner)
{
    Logger::start_tick();
    {
        // allocate and populate intent from concept 1
        // allocate a square matrix, intent has as much bitset vectors as bit in bitsets
        //std::cout << "concept1 extent size " << size << std::endl;
        runner->concept.intent = vector<std::bitset<$_COLUMN_NUMBER>>($_COLUMN_NUMBER);
        runner->concept.populateIntent();
    }
    Logger::end_tick("Initializing intent for concept 1 (Initializing)");

    return true;
}

