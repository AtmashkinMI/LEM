#ifndef GOSTOVERWRITER_HPP
#define GOSTOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class GostOverwriter : public Overwriter
{
public:
    GostOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~GostOverwriter();

private:
    FILE *randomSource;
};

#endif // GOSTOVERWRITER_HPP
