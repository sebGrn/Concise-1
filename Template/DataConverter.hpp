#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include "Bitset_Concept.hpp"

using std::ifstream;
using std::map;

//#include "util.hpp"

/// \brief Static Data converter from transactional data to binary data, can be called from anywhere
///
/// Use lambda function to extract vector of int from data
/// then apply another lambda function to transform the datas into appropriate format
class DataConverter
{
    /// \brief split a string into list of string with a delimiter
    static std::vector<std::string> split(const std::string& s, char delim);
    static std::vector<std::string> split2(const std::string& s, char delim, map<string, int>& dataStringToInt, map<int, string>& dataIntToString);

    /// \brief iterate on each line of a file and apply a lambda function for each line
    static bool iterate(const std::string& inputFilename, std::function<void(std::string&)> callback);

public:
    /// \brief extract vector of int from file
    static std::vector<std::vector<unsigned int>> extractIndexes(const std::string& inputFilename, unsigned int& indexMax);

    /// \brief extract vector of int from file
    static std::vector<std::vector<unsigned int>> extractIndexes2(const std::string& inputFilename, unsigned int& indexMax,
            map<string, int>& dataStringToInt, map<int, string>& dataIntToString);
};

template <typename Out>
inline void split(const std::string& s, char delim, Out result);
template <typename Out>
inline void split2(const std::string& s, char delim, map<string, int>& dataStringToInt, map<int, string>& dataIntToString, Out result);

std::vector<unsigned int> splitToInt(const std::string& s, char delim);
std::vector<unsigned int> splitToInt2(const std::string& s, char delim, map<string, int>& dataStringToInt, map<int, string>& dataIntToString);

