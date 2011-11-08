#include <string.h>

#include <QtGlobal>

#include "hmgbaseoverwriter.hpp"

HMGBaseOverwriter::HMGBaseOverwriter()
{
}

HMGBaseOverwriter::~HMGBaseOverwriter()
{
}

int HMGBaseOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification HMGBaseOverwriter::verificationType()
{
    return LastRound;
}

bool HMGBaseOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void HMGBaseOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    memset(block, '\x00', blockSize);
}
