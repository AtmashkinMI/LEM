#ifndef USDODEOVERWRITER_HPP
#define USDODEOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class USDoDEOverwriter : public Overwriter
{
public:
    USDoDEOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~USDoDEOverwriter();

private:
    char specChar;

    FILE *randomSource;
};

#endif // USDODEOVERWRITER_HPP
