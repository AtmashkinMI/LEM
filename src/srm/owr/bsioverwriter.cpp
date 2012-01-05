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

#include "bsioverwriter.hpp"

BSIOverwriter::BSIOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

BSIOverwriter::~BSIOverwriter()
{
    fclose(randomSource);
}

int BSIOverwriter::getRounds()
{
    return 6;
}

Overwriter::Verification BSIOverwriter::verificationType()
{
    return No;
}

QString BSIOverwriter::getName()
{
    return QObject::tr("German BSI IT BPM");
}

QString BSIOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten with certain bytes and with their complements through 6 passes without verification.");
}

bool BSIOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void BSIOverwriter::fillBlock(char *block, int size, int round)
{
    switch (round) {
        case 0:
            specChars[0] = getc(randomSource);
            memset(block, specChars[0], size);
            break;
        case 1:
            memset(block, ~specChars[0], size);
            break;
        case 2:
            specChars[1] = getc(randomSource);
            memset(block, specChars[1], size);
            break;
        case 3:
            memset(block, ~specChars[1], size);
            break;
        case 4:
            specChars[2] = getc(randomSource);
            memset(block, specChars[2], size);
            break;
        case 5:
            memset(block, ~specChars[2], size);
            break;
    }
}
