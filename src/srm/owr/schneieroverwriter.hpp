#ifndef SCHNEIEROVERWRITER_HPP
#define SCHNEIEROVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class SchneierOverwriter : public Overwriter
{
public:
    SchneierOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~SchneierOverwriter();

private:
    FILE *randomSource;
};

#endif // SCHNEIEROVERWRITER_HPP
