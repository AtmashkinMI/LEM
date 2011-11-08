#include <QtGlobal>

#include "pfitzneroverwriter.hpp"

PfitznerOverwriter::PfitznerOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

PfitznerOverwriter::~PfitznerOverwriter()
{
    fclose(randomSource);
}

int PfitznerOverwriter::getRounds()
{
    return 33;
}

Overwriter::Verification PfitznerOverwriter::verificationType()
{
    return No;
}

bool PfitznerOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return false;
}

void PfitznerOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    fread(block, 1, blockSize, randomSource);
}
