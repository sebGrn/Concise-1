#include <string>

#include "Bitset_Concept.hpp"

BitsetConcept::BitsetConcept()
{
    this->extent = vector<std::bitset<$_LINE_NUMBER>>($_COLUMN_NUMBER);
    this->intent = vector<std::bitset<$_COLUMN_NUMBER>>($_COLUMN_NUMBER);
}

BitsetConcept::~BitsetConcept()
{
}

void BitsetConcept::populateIntent()
{
    size_t n = static_cast<int>(this->extent.size());
    size_t nbIter = n * n;
//#pragma omp parallel for
    for (size_t i = 0; i < nbIter; i++)
    {
        int row = static_cast<int>(i / n);
        int col = static_cast<int>(i % n);
        this->intent[row][col] = (this->extent[row] & ~this->extent[col]).none();
    }
}
