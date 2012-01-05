/*
   Copyright (C) 2011 by Atmashkin M.I. All Rights Reserved.

   This file is part of LEM.

   LEM is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   LEM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with LEM. If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "navsorlloverwriter.hpp"

const char NAVSORLLOverwriter::pattern[4] = {'\x27', '\xff', '\xff', '\xff'};

NAVSORLLOverwriter::NAVSORLLOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

NAVSORLLOverwriter::~NAVSORLLOverwriter()
{
    fclose(randomSource);
}

int NAVSORLLOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification NAVSORLLOverwriter::verificationType()
{
    return LastRound;
}

QString NAVSORLLOverwriter::getName()
{
    return QObject::tr("US NAVSO P-5239-26 (RLL)");
}

QString NAVSORLLOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0xFF' bytes, second pass - with special RLL pattern and the last pass - with random bytes.");
}

bool NAVSORLLOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void NAVSORLLOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\xff', blockSize);
            break;
        case 1:
            for (int i = 0; i < blockSize / 4; ++i) {
                *block++ = pattern[0];
                *block++ = pattern[1];
                *block++ = pattern[2];
                *block++ = pattern[3];
            }
            break;
        case 2:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
    }
}
