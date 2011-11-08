#include <string.h>

#include "gostoverwriter.hpp"

GostOverwriter::GostOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

GostOverwriter::~GostOverwriter()
{
    fclose(randomSource);
}

int GostOverwriter::getRounds()
{
    return 2;
}

Overwriter::Verification GostOverwriter::verificationType()
{
    return No;
}

bool GostOverwriter::isConstantRound(int round)
{
    return round == 1 ? false : true;
}

void GostOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\x00', blockSize);
            break;
        case 1:
            fread(block, 1, blockSize, randomSource);
            break;
    }
}
