#include <string.h>

#include "usarmyoverwriter.hpp"

USArmyOverwriter::USArmyOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USArmyOverwriter::~USArmyOverwriter()
{
    fclose(randomSource);
}

int USArmyOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification USArmyOverwriter::verificationType()
{
    return LastRound;
}

bool USArmyOverwriter::isConstantRound(int round)
{
    return round == 0 ? false : true;
}

void USArmyOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
        case 1:
            specChar = getc(randomSource);
            memset(block, specChar, blockSize);
            break;
        case 2:
            memset(block, ~specChar, blockSize);
            break;
    }
}
