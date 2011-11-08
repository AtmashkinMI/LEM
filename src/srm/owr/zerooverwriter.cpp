#include <string.h>

#include <QtGlobal>

#include "zerooverwriter.hpp"

ZeroOverwriter::ZeroOverwriter()
{
}

ZeroOverwriter::~ZeroOverwriter()
{
}

int ZeroOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification ZeroOverwriter::verificationType()
{
    return No;
}

bool ZeroOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void ZeroOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    memset(block, '\x00', blockSize);
}
