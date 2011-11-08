#include <string.h>

#include "navsorlloverwriter.hpp"

const char NAVSORLLOverwriter::pattern[4] = {'\x27', '\xff', '\xff', '\xff'};

NAVSORLLOverwriter::NAVSORLLOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

NAVSORLLOverwriter::~NAVSORLLOverwriter()
{
    fclose(randomSource);
}

int NAVSORLLOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification NAVSORLLOverwriter::verificationType()
{
    return LastRound;
}

bool NAVSORLLOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void NAVSORLLOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\xff', blockSize);
            break;
        case 1:
            for (int i = 0; i < blockSize / 4; ++i) {
                *block++ = pattern[0];
                *block++ = pattern[1];
                *block++ = pattern[2];
                *block++ = pattern[3];
            }
            break;
        case 2:
            fread(block, 1, blockSize, randomSource);
            break;
    }
}
