#include <string.h>

#include "hmgenhancedoverwriter.hpp"

HMGEnhancedOverwriter::HMGEnhancedOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

HMGEnhancedOverwriter::~HMGEnhancedOverwriter()
{
    fclose(randomSource);
}

int HMGEnhancedOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification HMGEnhancedOverwriter::verificationType()
{
    return LastRound;
}

bool HMGEnhancedOverwriter::isConstantRound(int round)
{
   return round == 2 ? false : true;
}

void HMGEnhancedOverwriter::fillBlock(char *block, int blockSize, int round)
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
