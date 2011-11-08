#ifndef ONEOVERWRITER_HPP
#define ONEOVERWRITER_HPP

#include "overwriter.hpp"

class OneOverwriter : public Overwriter
{
public:
    OneOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~OneOverwriter();
};

#endif // ONEOVERWRITER_HPP
