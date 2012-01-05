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

#include <QtGlobal>

#include "afssioverwriter.hpp"

AFSSIOverwriter::AFSSIOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

AFSSIOverwriter::~AFSSIOverwriter()
{
    fclose(randomSource);
}

int AFSSIOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification AFSSIOverwriter::verificationType()
{
    return LastRound;
}

QString AFSSIOverwriter::getName()
{
    return QObject::tr("US Air Force AFSSI-5020");
}

QString AFSSIOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0x00' bytes, second pass - with '0xFF' bytes and the last pass - with certain byte.");
}

bool AFSSIOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void AFSSIOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\x00', blockSize);
            break;
        case 1:
            memset(block, '\xff', blockSize);
            break;
        case 2:
            memset(block, getc(randomSource), blockSize);
            break;
    }
}
