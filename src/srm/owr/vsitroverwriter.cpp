#include <string.h>

#include <QtGlobal>

#include "vsitroverwriter.hpp"

VSITROverwriter::VSITROverwriter()
{
}

VSITROverwriter::~VSITROverwriter()
{
}

int VSITROverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification VSITROverwriter::verificationType()
{
    return No;
}

bool VSITROverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void VSITROverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
        case 2:
        case 4:
            memset(block, '\x00', blockSize);
            break;
        case 1:
        case 3:
        case 5:
            memset(block, '\xff', blockSize);
            break;
        case 6:
            memset(block, '\xaa', blockSize);
            break;
    }
}
