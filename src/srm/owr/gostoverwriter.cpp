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

#include "gostoverwriter.hpp"

GostOverwriter::GostOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

GostOverwriter::~GostOverwriter()
{
    fclose(randomSource);
}

int GostOverwriter::getRounds()
{
    return 2;
}

Overwriter::Verification GostOverwriter::verificationType()
{
    return No;
}

QString GostOverwriter::getName()
{
    return QObject::tr("Russian State Technical Commission - 1992");
}

QString GostOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 2 passes without verification: first pass - with '0x00' bytes and the last pass - with random bytes.");
}

bool GostOverwriter::isConstantRound(int round)
{
    return round == 1 ? false : true;
}

void GostOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
            memset(block, '\x00', blockSize);
            break;
        case 1:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
    }
}
