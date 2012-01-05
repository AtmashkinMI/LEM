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

#include "usdodeoverwriter.hpp"

USDoDEOverwriter::USDoDEOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

USDoDEOverwriter::~USDoDEOverwriter()
{
    fclose(randomSource);
}

int USDoDEOverwriter::getRounds()
{
    return 3;
}

Overwriter::Verification USDoDEOverwriter::verificationType()
{
    return No;
}

QString USDoDEOverwriter::getName()
{
    return QObject::tr("US Department of Defense 5220.22-M (E)");
}

QString USDoDEOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 3 passes without verification: 2 passes - with certain byte and with its complement and the last pass - with random bytes.");
}

bool USDoDEOverwriter::isConstantRound(int round)
{
    return round == 2 ? false : true;
}

void USDoDEOverwriter::fillBlock(char *block, int size, int round)
{
    switch (round) {
        case 0:
            specChar = getc(randomSource);
            memset(block, specChar, size);
            break;
        case 1:
            memset(block, ~specChar, size);
            break;
        case 2:
            for (int count = 0; count < size; count += fread(block, 1, size - count, randomSource));
            break;
    }
}
