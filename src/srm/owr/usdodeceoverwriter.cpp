#include <string.h>

#include "usdodeceoverwriter.hpp"

USDoDECEOverwriter::USDoDECEOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USDoDECEOverwriter::~USDoDECEOverwriter()
{
    fclose(randomSource);
}

int USDoDECEOverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification USDoDECEOverwriter::verificationType()
{
    return No;
}

bool USDoDECEOverwriter::isConstantRound(int round)
{
    return round == 2 || round == 3 || round == 6 ? false : true;
}

void USDoDECEOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            specChars[0] = getc(randomSource);
            memset(block, specChars[0], blockSize);
            break;
        case 1:
            memset(block, ~specChars[0], blockSize);
            break;
        case 4:
            specChars[1] = getc(randomSource);
            memset(block, specChars[1], blockSize);
            break;
        case 5:
            memset(block, ~specChars[1], blockSize);
            break;
        case 2:
        case 3:
        case 6:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
    }
}
