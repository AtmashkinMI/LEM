#include <string.h>

#include "rcmpoverwriter.hpp"

RCMPOverwriter::RCMPOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

RCMPOverwriter::~RCMPOverwriter()
{
    fclose(randomSource);
}

int RCMPOverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification RCMPOverwriter::verificationType()
{
    return LastRound;
}

bool RCMPOverwriter::isConstantRound(int round)
{
    return round == 6 ? false : true;
}

void RCMPOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
        case 2:
        case 4:
            memset(block, '\x00', blockSize);
            break;
        case 1:
        case 3:
        case 5:
            memset(block, '\xff', blockSize);
            break;
        case 6:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
    }
}
