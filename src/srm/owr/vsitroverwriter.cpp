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

#include "vsitroverwriter.hpp"

VSITROverwriter::VSITROverwriter()
{
}

VSITROverwriter::~VSITROverwriter()
{
}

int VSITROverwriter::getRounds()
{
    return 7;
}

Overwriter::Verification VSITROverwriter::verificationType()
{
    return No;
}

QString VSITROverwriter::getName()
{
    return QObject::tr("German VSITR");
}

QString VSITROverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 7 passes without verification: 6 alternating passes - with '0x00' bytes and '0xFF' bytes and the last pass - with '0xAA' bytes.");
}

bool VSITROverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void VSITROverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
        case 2:
        case 4:
            memset(block, '\x00', blockSize);
            break;
        case 1:
        case 3:
        case 5:
            memset(block, '\xff', blockSize);
            break;
        case 6:
            memset(block, '\xaa', blockSize);
            break;
    }
}
