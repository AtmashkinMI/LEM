#ifndef USDODOVERWRITER_HPP
#define USDODOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class USDoDOverwriter : public Overwriter
{
public:
    USDoDOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~USDoDOverwriter();

private:
    FILE *randomSource;
};

#endif // USDODOVERWRITER_HPP
