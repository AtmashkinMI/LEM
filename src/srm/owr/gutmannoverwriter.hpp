#ifndef GUTMANNOVERWRITER_HPP
#define GUTMANNOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class GutmannOverwriter : public Overwriter
{
public:
    GutmannOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~GutmannOverwriter();

private:
    FILE *randomSource;

    static const char patterns[27][3];
};

#endif // GUTMANNOVERWRITER_HPP
