#ifndef PFITZNEROVERWRITER_HPP
#define PFITZNEROVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class PfitznerOverwriter : public Overwriter
{
public:
    PfitznerOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~PfitznerOverwriter();

private:
    FILE *randomSource;
};

#endif // PFITZNEROVERWRITER_HPP
