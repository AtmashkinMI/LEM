#ifndef HMGENHANCEDOVERWRITER_HPP
#define HMGENHANCEDOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class HMGEnhancedOverwriter : public Overwriter
{
public:
    HMGEnhancedOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~HMGEnhancedOverwriter();

private:
    FILE *randomSource;
};

#endif // HMGENHANCEDOVERWRITER_HPP
