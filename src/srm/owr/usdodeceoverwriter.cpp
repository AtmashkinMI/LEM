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

#include "usdodeceoverwriter.hpp"

USDoDECEOverwriter::USDoDECEOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USDoDECEOverwriter::~USDoDECEOverwriter()
{
    fclose(randomSource);
}

int USDoDECEOverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification USDoDECEOverwriter::verificationType()
{
    return No;
}

QString USDoDECEOverwriter::getName()
{
    return QObject::tr("US Department of Defense 5220.22-M (ECE)");
}

QString USDoDECEOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 7 passes without verification: 2 passes - with certain byte and with its complement, then 2 passes - with random bytes, then 2 passes - with certain byte and with its complement and the last pass - with random bytes.");
}

bool USDoDECEOverwriter::isConstantRound(int round)
{
    return round == 2 || round == 3 || round == 6 ? false : true;
}

void USDoDECEOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            specChars[0] = getc(randomSource);
            memset(block, specChars[0], blockSize);
            break;
        case 1:
            memset(block, ~specChars[0], blockSize);
            break;
        case 4:
            specChars[1] = getc(randomSource);
            memset(block, specChars[1], blockSize);
            break;
        case 5:
            memset(block, ~specChars[1], blockSize);
            break;
        case 2:
        case 3:
        case 6:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
    }
}
