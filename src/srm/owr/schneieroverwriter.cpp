#include <string.h>

#include "schneieroverwriter.hpp"

SchneierOverwriter::SchneierOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

SchneierOverwriter::~SchneierOverwriter()
{
    fclose(randomSource);
}

int SchneierOverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification SchneierOverwriter::verificationType()
{
    return No;
}

bool SchneierOverwriter::isConstantRound(int round)
{
    return round >= 2 ? false : true;
}

void SchneierOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\xff', blockSize);
            break;
        case 1:
            memset(block, '\x00', blockSize);
            break;
        default:
            fread(block, 1, blockSize, randomSource);
            break;
    }
}
