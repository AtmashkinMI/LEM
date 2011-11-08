#include <string.h>

#include <QtGlobal>

#include "bsioverwriter.hpp"

BSIOverwriter::BSIOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

BSIOverwriter::~BSIOverwriter()
{
    fclose(randomSource);
}

int BSIOverwriter::getRounds()
{
    return 6;
}

Overwriter::Verification BSIOverwriter::verificationType()
{
    return No;
}

bool BSIOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void BSIOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            specChars[0] = getc(randomSource);
            memset(block, specChars[0], blockSize);
            break;
        case 1:
            memset(block, ~specChars[0], blockSize);
            break;
        case 2:
            specChars[1] = getc(randomSource);
            memset(block, specChars[1], blockSize);
            break;
        case 3:
            memset(block, ~specChars[1], blockSize);
            break;
        case 4:
            specChars[2] = getc(randomSource);
            memset(block, specChars[2], blockSize);
            break;
        case 5:
            memset(block, ~specChars[2], blockSize);
            break;
    }
}
