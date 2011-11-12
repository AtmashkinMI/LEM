#include <string.h>

#include "navsomfmoverwriter.hpp"

const char NAVSOMFMOverwriter::pattern[4] = {'\x7f', '\xff', '\xff', '\xff'};

NAVSOMFMOverwriter::NAVSOMFMOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

NAVSOMFMOverwriter::~NAVSOMFMOverwriter()
{
    fclose(randomSource);
}

int NAVSOMFMOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification NAVSOMFMOverwriter::verificationType()
{
    return LastRound;
}

bool NAVSOMFMOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void NAVSOMFMOverwriter::fillBlock(char *block, int blockSize, int round)
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
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
    }
}
