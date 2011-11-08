#ifndef OVERWRITER_HPP
#define OVERWRITER_HPP

class Overwriter
{
public:
    Overwriter();

    virtual int getRounds() = 0;
    virtual bool isConstantRound(int round) = 0;
    virtual void fillBlock(char *block, int blockSize, int round) = 0;

    enum Verification {No, LastRound, AllRouns};

    virtual Verification verificationType() = 0;

    virtual ~Overwriter();
};

#endif // OVERWRITER_HPP
