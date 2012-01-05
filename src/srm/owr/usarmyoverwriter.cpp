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

#include "usarmyoverwriter.hpp"

USArmyOverwriter::USArmyOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USArmyOverwriter::~USArmyOverwriter()
{
    fclose(randomSource);
}

int USArmyOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification USArmyOverwriter::verificationType()
{
    return LastRound;
}

QString USArmyOverwriter::getName()
{
    return QObject::tr("US Army AR380-19");
}

QString USArmyOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 3 passes with verification of last pass: first pass - with random bytes, then 2 passes - with certain byte and with its complement.");
}

bool USArmyOverwriter::isConstantRound(int round)
{
    return round == 0 ? false : true;
}

void USArmyOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
        case 1:
            specChar = getc(randomSource);
            memset(block, specChar, blockSize);
            break;
        case 2:
            memset(block, ~specChar, blockSize);
            break;
    }
}
