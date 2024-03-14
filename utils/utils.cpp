#include "utils.h"

int calcNumBits(double lowerBound, double upperBound, int decimalBits) {
    if (lowerBound > upperBound) {
        throw std::invalid_argument("Lower bound should be less than or equal to upper bound.");
    }

    double maxAbsValue = std::max(std::abs(lowerBound), std::abs(upperBound));
    int bits = std::ceil(std::log2(maxAbsValue + 1)); 

    int signBit = (lowerBound < 0) ? 1 : 0;

    int totalBits = bits + signBit + decimalBits;

    if (totalBits <= 8) return 8;
    if (totalBits <= 16) return 16;
    if (totalBits <= 32) return 32;
    if (totalBits <= 64) return 64;

    throw std::range_error("The number range is too large to be represented within 64 bits.");
}
