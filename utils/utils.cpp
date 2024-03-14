#include <cmath>
#include <iostream>

int calcNumBits(double upperBound) {

    if (upperBound <= 0) {
        return 1;
    }

    int numBits = std::ceil(std::log2(upperBound + 1));

    return numBits;
}
