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

#include "hmgbaseoverwriter.hpp"

HMGBaseOverwriter::HMGBaseOverwriter()
{
}

HMGBaseOverwriter::~HMGBaseOverwriter()
{
}

int HMGBaseOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification HMGBaseOverwriter::verificationType()
{
    return LastRound;
}

QString HMGBaseOverwriter::getName()
{
    return QObject::tr("British HMG IS5 (Baseline)");
}

QString HMGBaseOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten with '0x00' bytes through 1 pass with verification.");
}

bool HMGBaseOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return true;
}

void HMGBaseOverwriter::fillBlock(char *block, int size, int round)
{
    Q_UNUSED(round)

    memset(block, '\x00', size);
}
