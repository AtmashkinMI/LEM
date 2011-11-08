#ifndef VSITROVERWRITER_HPP
#define VSITROVERWRITER_HPP

#include "overwriter.hpp"

class VSITROverwriter : public Overwriter
{
public:
    VSITROverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    Verification verificationType();

    ~VSITROverwriter();
};

#endif // VSITROVERWRITER_HPP
