#ifndef HMGBASEOVERWRITER_HPP
#define HMGBASEOVERWRITER_HPP

#include "overwriter.hpp"

class HMGBaseOverwriter : public Overwriter
{
public:
    HMGBaseOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~HMGBaseOverwriter();
};

#endif // HMGBASEOVERWRITER_HPP
