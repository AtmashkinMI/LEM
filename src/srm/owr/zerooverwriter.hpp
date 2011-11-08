#ifndef ZEROOVERWRITER_HPP
#define ZEROOVERWRITER_HPP

#include "overwriter.hpp"

class ZeroOverwriter : public Overwriter
{
public:
    ZeroOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~ZeroOverwriter();
};

#endif // ZEROOVERWRITER_HPP
