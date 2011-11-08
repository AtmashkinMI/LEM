#include <string.h>

#include <QtGlobal>

#include "afssioverwriter.hpp"

AFSSIOverwriter::AFSSIOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

AFSSIOverwriter::~AFSSIOverwriter()
{
    fclose(randomSource);
}

int AFSSIOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification AFSSIOverwriter::verificationType()
{
    return LastRound;
}

bool AFSSIOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void AFSSIOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\x00', blockSize);
            break;
        case 1:
            memset(block, '\xff', blockSize);
            break;
        case 2:
            memset(block, getc(randomSource), blockSize);
            break;
    }
}
