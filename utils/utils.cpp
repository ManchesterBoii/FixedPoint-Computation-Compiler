#include <cmath>
#include <iostream>

int calcNumBits(double lowerBound, double upperBound, int decimalBits) {
    if (lowerBound > upperBound) {
        throw std::invalid_argument("Lower bound should be less than or equal to upper bound.");
    }

    int maxAbsValue = std::max(std::abs(lowerBound), std::abs(upperBound));
    int bits = std::ceil(std::log2(maxAbsValue + 1)); 

    // sign bit
    bits++;

    // precision bits
    bits += decimalBits;

    if (bits <= 8) return 8;
    if (bits <= 16) return 16;
    if (bits <= 32) return 32;
    if (bits <= 64) return 64;

    throw std::range_error("The number range is too large to be represented within 64 bits.");
}