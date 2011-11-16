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

#include <QtGlobal>

#include "ismoverwriter.hpp"

ISMOverwriter::ISMOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

ISMOverwriter::~ISMOverwriter()
{
    fclose(randomSource);
}

int ISMOverwriter::getRounds()
{
    return 1;
}

Overwriter::Verification ISMOverwriter::verificationType()
{
    return LastRound;
}

bool ISMOverwriter::isConstantRound(int round)
{
    Q_UNUSED(round)

    return false;
}

void ISMOverwriter::fillBlock(char *block, int blockSize, int round)
{
    Q_UNUSED(round)

    for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
}
