#include <QtGlobal>

#include "randomoverwriter.hpp"

RandomOverwriter::RandomOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

RandomOverwriter::~RandomOverwriter()
{
    fclose(randomSource);
}

int RandomOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification RandomOverwriter::verificationType()
{
    return No;
}

bool RandomOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return false;
}

void RandomOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    fread(block, 1, blockSize, randomSource);
}
