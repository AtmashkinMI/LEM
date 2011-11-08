#ifndef AFSSIOVERWRITER_HPP
#define AFSSIOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class AFSSIOverwriter : public Overwriter
{
public:
    AFSSIOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~AFSSIOverwriter();

private:
    FILE *randomSource;
};

#endif // AFSSIOVERWRITER_HPP
