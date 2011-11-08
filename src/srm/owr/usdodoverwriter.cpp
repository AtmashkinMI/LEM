#include <string.h>

#include "usdodoverwriter.hpp"

USDoDOverwriter::USDoDOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USDoDOverwriter::~USDoDOverwriter()
{
    fclose(randomSource);
}

int USDoDOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification USDoDOverwriter::verificationType()
{
    return AllRouns;
}

bool USDoDOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void USDoDOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\x00', blockSize);
            break;
        case 1:
            memset(block, '\xff', blockSize);
            break;
        case 2:
            fread(block, 1, blockSize, randomSource);
            break;
    }
}
