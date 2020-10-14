#include <map>
#include "DataConverter.hpp"

// -------------------------------------------------------------------------------------------------------------------------------------- //

template <typename Out>
void split2(const std::string& s, char delim, map<string, int>& dataStringToInt, map<int, string>& dataIntToString, Out result)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim))
        if (dataStringToInt.find(item) != dataStringToInt.end()) {
            // Use existing value
            *result++ = dataStringToInt.at(item);
        } else {
            int new_value = dataStringToInt.size() + 1;
            dataStringToInt[item] = new_value;
            dataIntToString[new_value] = item;
            *result++ = new_value;
        }
}

std::vector<unsigned int> splitToInt2(const std::string& s, char delim, map<string, int>& dataStringToInt,
                                      map<int, string>& dataIntToString)
{
    std::vector<unsigned int> elems;
    split2(s, delim, dataStringToInt, dataIntToString, std::back_inserter(elems));
    return elems;
}

// -------------------------------------------------------------------------------------------------------------------------------------- //

std::vector<std::vector<unsigned int>> DataConverter::extractIndexes2(const std::string& inputFilename, unsigned int& indexMax,
                                    map<string, int>& dataStringToInt, map<int, string>& dataIntToString)
{
    std::vector<std::vector<unsigned int>> vecOfInt;
    auto lambdaPopulateStrVector = [&](std::string& line) {

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if(line[0] == ' ')
            line.erase(0, 1);
        if(line[line.size() - 1] == ' ')
            line.pop_back();
        std::vector<unsigned int> data = splitToInt2(line, ' ', dataStringToInt, dataIntToString);
        indexMax = std::max(*std::max_element(data.begin(), data.end()), indexMax);

        vecOfInt.push_back(data);
    };

    iterate(inputFilename, lambdaPopulateStrVector);
    return vecOfInt;
}

template <typename Out>
void split(const std::string& s, char delim, Out result)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim))
        *result++ = std::stoi(item);
}

std::vector<unsigned int> splitToInt(const std::string& s, char delim)
{
    std::vector<unsigned int> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::vector<std::vector<unsigned int>> DataConverter::extractIndexes(const std::string& inputFilename, unsigned int& indexMax)
{
    std::vector<std::vector<unsigned int>> vecOfInt;
    auto lambdaPopulateStrVector = [&](std::string& line) {

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if(line[0] == ' ')
            line.erase(0, 1);
        if(line[line.size() - 1] == ' ')
            line.pop_back();
        std::vector<unsigned int> data = splitToInt(line, ' ');
        indexMax = std::max(*std::max_element(data.begin(), data.end()), indexMax);

        vecOfInt.push_back(data);
    };

    iterate(inputFilename, lambdaPopulateStrVector);
    return vecOfInt;
}

bool DataConverter::iterate(const std::string& inputFilename, std::function<void(std::string&)> callback)
{
    ifstream inputStream(inputFilename, ifstream::in);

    std::string line;
    while (std::getline(inputStream, line))
        callback(line);

    inputStream.close();
    return true;
}

