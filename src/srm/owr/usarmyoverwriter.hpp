#ifndef USARMYOVERWRITER_HPP
#define USARMYOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class USArmyOverwriter : public Overwriter
{
public:
    USArmyOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~USArmyOverwriter();

private:
    char specChar;

    FILE *randomSource;
};

#endif // USARMYOVERWRITER_HPP
