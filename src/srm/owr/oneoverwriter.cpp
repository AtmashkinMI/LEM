#include <string.h>

#include <QtGlobal>

#include "oneoverwriter.hpp"

OneOverwriter::OneOverwriter()
{
}

OneOverwriter::~OneOverwriter()
{
}

int OneOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification OneOverwriter::verificationType()
{
    return No;
}

bool OneOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void OneOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    memset(block, '\xff', blockSize);
}
