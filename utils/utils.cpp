#include <cmath>
#include <iostream>

int calcNumBits(int upperBound) {

    if (upperBound <= 0) {
        return 1;
    }

    int numBits = std::ceil(std::log2(static_cast<double>(upperBound) + 1));

    return numBits;
}
