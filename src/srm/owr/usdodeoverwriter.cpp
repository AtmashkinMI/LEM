#include <string.h>

#include "usdodeoverwriter.hpp"

USDoDEOverwriter::USDoDEOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USDoDEOverwriter::~USDoDEOverwriter()
{
    fclose(randomSource);
}

int USDoDEOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification USDoDEOverwriter::verificationType()
{
    return No;
}

bool USDoDEOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void USDoDEOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            specChar = getc(randomSource);
            memset(block, specChar, blockSize);
            break;
        case 1:
            memset(block, ~specChar, blockSize);
            break;
        case 2:
            fread(block, 1, blockSize, randomSource);
            break;
    }
}
