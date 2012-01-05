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

#include "navsomfmoverwriter.hpp"

const char NAVSOMFMOverwriter::pattern[4] = {'\x7f', '\xff', '\xff', '\xff'};

NAVSOMFMOverwriter::NAVSOMFMOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

NAVSOMFMOverwriter::~NAVSOMFMOverwriter()
{
    fclose(randomSource);
}

int NAVSOMFMOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification NAVSOMFMOverwriter::verificationType()
{
    return LastRound;
}

QString NAVSOMFMOverwriter::getName()
{
    return QObject::tr("US NAVSO P-5239-26 (MFM)");
}

QString NAVSOMFMOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0xFF' bytes, second pass - with special MFM pattern and the last pass - with random bytes.");
}

bool NAVSOMFMOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void NAVSOMFMOverwriter::fillBlock(char *block, int size, int round)
{
    switch (round) {
        case 0:
            memset(block, '\xff', size);
            break;
        case 1:
            for (int i = 0; i < size / 4; ++i) {
                *block++ = pattern[0];
                *block++ = pattern[1];
                *block++ = pattern[2];
                *block++ = pattern[3];
            }
            break;
        case 2:
            for (int count = 0; count < size; count += fread(block, 1, size - count, randomSource));
            break;
    }
}
