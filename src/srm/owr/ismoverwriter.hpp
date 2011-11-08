#ifndef ISMOVERWRITER_HPP
#define ISMOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class ISMOverwriter : public Overwriter
{
public:
    ISMOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~ISMOverwriter();

private:
    FILE *randomSource;
};

#endif // ISMOVERWRITER_HPP
