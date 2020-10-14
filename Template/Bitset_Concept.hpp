#pragma once
#include <vector>
#include <bitset>
#include <cassert>
#include <algorithm> 
#include <string> 
#include <iostream> 
#include <fstream>

using std::string;
using std::vector;
using std::ofstream;

class BitsetConcept
{
public:
	// extent matrix definition
	vector<std::bitset<$_LINE_NUMBER>> extent;

    // extent matrix definition not reversed
    vector<std::bitset<$_COLUMN_NUMBER>> extentNotReversed;
	
	// intent matrix definition
	vector<std::bitset<$_COLUMN_NUMBER>> intent;
	
	vector<unsigned int> support;

public:
	BitsetConcept();
	~BitsetConcept();

	void populateIntent();
};

