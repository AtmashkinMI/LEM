#ifndef RANDOMOVERWRITER_HPP
#define RANDOMOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class RandomOverwriter : public Overwriter
{
public:
    RandomOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~RandomOverwriter();

private:
    FILE *randomSource;
};

#endif // RANDOMOVERWRITER_HPP
