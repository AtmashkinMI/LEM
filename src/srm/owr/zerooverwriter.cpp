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

#include "zerooverwriter.hpp"

ZeroOverwriter::ZeroOverwriter()
{
}

ZeroOverwriter::~ZeroOverwriter()
{
}

int ZeroOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification ZeroOverwriter::verificationType()
{
    return No;
}

QString ZeroOverwriter::getName()
{
    return QObject::tr("Overwrite with zeros");
}

QString ZeroOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten with '0x00' bytes through 1 pass without verification.");
}

bool ZeroOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void ZeroOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    memset(block, '\x00', blockSize);
}
