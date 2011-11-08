#include <QtGlobal>

#include "ismoverwriter.hpp"

ISMOverwriter::ISMOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

ISMOverwriter::~ISMOverwriter()
{
    fclose(randomSource);
}

int ISMOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification ISMOverwriter::verificationType()
{
    return LastRound;
}

bool ISMOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return false;
}

void ISMOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    fread(block, 1, blockSize, randomSource);
}
