#ifndef RCMPOVERWRITER_HPP
#define RCMPOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class RCMPOverwriter : public Overwriter
{
public:
    RCMPOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~RCMPOverwriter();

private:
    FILE *randomSource;
};

#endif // RCMPOVERWRITER_HPP
