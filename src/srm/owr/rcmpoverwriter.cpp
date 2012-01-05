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

#include "rcmpoverwriter.hpp"

RCMPOverwriter::RCMPOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

RCMPOverwriter::~RCMPOverwriter()
{
    fclose(randomSource);
}

int RCMPOverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification RCMPOverwriter::verificationType()
{
    return LastRound;
}

QString RCMPOverwriter::getName()
{
    return QObject::tr("Canadian RCMP TSSIT OPS-II");
}

QString RCMPOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 7 passes with verification of last pass: 6 alternating passes - with '0x00' bytes and '0xFF' bytes and the last pass - with random bytes.");
}

bool RCMPOverwriter::isConstantRound(int round)
{
    return round == 6 ? false : true;
}

void RCMPOverwriter::fillBlock(char *block, int size, int round)
{
    switch (round) {
        case 0:
        case 2:
        case 4:
            memset(block, '\x00', size);
            break;
        case 1:
        case 3:
        case 5:
            memset(block, '\xff', size);
            break;
        case 6:
            for (int count = 0; count < size; count += fread(block, 1, size - count, randomSource));
            break;
    }
}
