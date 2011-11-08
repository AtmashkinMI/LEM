#ifndef NAVSORLLOVERWRITER_HPP
#define NAVSORLLOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class NAVSORLLOverwriter : public Overwriter
{
public:
    NAVSORLLOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~NAVSORLLOverwriter();

private:
    FILE *randomSource;

    static const char pattern[4];
};

#endif // NAVSORLLOVERWRITER_HPP
