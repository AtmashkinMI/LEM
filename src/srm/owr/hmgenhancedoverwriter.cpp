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

#include "hmgenhancedoverwriter.hpp"

HMGEnhancedOverwriter::HMGEnhancedOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

HMGEnhancedOverwriter::~HMGEnhancedOverwriter()
{
    fclose(randomSource);
}

int HMGEnhancedOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification HMGEnhancedOverwriter::verificationType()
{
    return LastRound;
}

QString HMGEnhancedOverwriter::getName()
{
    return QObject::tr("British HMG IS5 (Enhanced)");
}

QString HMGEnhancedOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0x00' bytes, second pass - with '0xFF' bytes and the last pass - with random bytes.");
}

bool HMGEnhancedOverwriter::isConstantRound(int round)
{
   return round == 2 ? false : true;
}

void HMGEnhancedOverwriter::fillBlock(char *block, int size, int round)
{
    switch (round) {
        case 0:
            memset(block, '\x00', size);
            break;
        case 1:
            memset(block, '\xff', size);
            break;
        case 2:
            for (int count = 0; count < size; count += fread(block, 1, size - count, randomSource));
            break;
    }
}
