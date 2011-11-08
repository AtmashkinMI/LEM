#ifndef USDODECEOVERWRITER_HPP
#define USDODECEOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class USDoDECEOverwriter : public Overwriter
{
public:
    USDoDECEOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~USDoDECEOverwriter();

private:
    char specChars[2];

    FILE *randomSource;
};

#endif // USDODECEOVERWRITER_HPP
