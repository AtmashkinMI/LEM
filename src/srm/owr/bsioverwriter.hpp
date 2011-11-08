#ifndef BSIOVERWRITER_HPP
#define BSIOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class BSIOverwriter : public Overwriter
{
public:
    BSIOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~BSIOverwriter();

private:
    char specChars[3];

    FILE *randomSource;
};

#endif // BSIOVERWRITER_HPP
