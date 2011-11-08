#ifndef MEMORYERASER_HPP
#define MEMORYERASER_HPP

class Overwriter;

class MemoryEraser
{
public:
    MemoryEraser(int owrType);
    ~MemoryEraser();

    int erase();

private:
    bool changeOOMScore();
    bool needCheck(int round);

    char *block;

    Overwriter *overwriter;
};

#endif // MEMORYERASER_HPP
