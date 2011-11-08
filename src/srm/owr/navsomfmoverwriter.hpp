#ifndef NAVSOMFMOVERWRITER_HPP
#define NAVSOMFMOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class NAVSOMFMOverwriter : public Overwriter
{
public:
    NAVSOMFMOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~NAVSOMFMOverwriter();

private:
    FILE *randomSource;

    static const char pattern[4];
};

#endif // NAVSOMFMOVERWRITER_HPP
