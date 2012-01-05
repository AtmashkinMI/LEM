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

#include "gutmannoverwriter.hpp"

const char GutmannOverwriter::patterns[27][3] = {
    {'\x55', '\x55', '\x55'}, {'\xaa', '\xaa', '\xaa'}, {'\x92', '\x49', '\x24'}, {'\x49', '\x24', '\x92'},
    {'\x24', '\x92', '\x49'}, {'\x00', '\x00', '\x00'}, {'\x11', '\x11', '\x11'}, {'\x22', '\x22', '\x22'},
    {'\x33', '\x33', '\x33'}, {'\x44', '\x44', '\x44'}, {'\x55', '\x55', '\x55'}, {'\x66', '\x66', '\x66'},
    {'\x77', '\x77', '\x77'}, {'\x88', '\x88', '\x88'}, {'\x99', '\x99', '\x99'}, {'\xaa', '\xaa', '\xaa'},
    {'\xbb', '\xbb', '\xbb'}, {'\xcc', '\xcc', '\xcc'}, {'\xdd', '\xdd', '\xdd'}, {'\xee', '\xee', '\xee'},
    {'\xff', '\xff', '\xff'}, {'\x92', '\x49', '\x24'}, {'\x49', '\x24', '\x92'}, {'\x24', '\x92', '\x49'},
    {'\x6d', '\xb6', '\xdb'}, {'\xb6', '\xdb', '\x6d'}, {'\xdb', '\x6d', '\xb6'}
};

GutmannOverwriter::GutmannOverwriter()
{
    randomSource = fopen(URANDOM_FILE, "r");
}

GutmannOverwriter::~GutmannOverwriter()
{
    fclose(randomSource);
}

int GutmannOverwriter::getRounds()
{
    return 35;
}

Overwriter::Verification GutmannOverwriter::verificationType()
{
    return No;
}

QString GutmannOverwriter::getName()
{
    return QObject::tr("Peter Gutmann's method");
}

QString GutmannOverwriter::getDescription()
{
    return QObject::tr("Your data is overwritten through 35 passes without verification: 4 passes - with random bytes, then 27 passes - with special Gutmann's patterns, then 4 passes - with random bytes.");
}

bool GutmannOverwriter::isConstantRound(int round)
{
    return round < 4 || round > 30 ? false : true;
}

void GutmannOverwriter::fillBlock(char *block, int blockSize, int round)
{
    switch (round) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 31:
        case 32:
        case 33:
        case 34:
            for (int count = 0; count < blockSize; count += fread(block, 1, blockSize - count, randomSource));
            break;
        default:
            for (int i = 0; i < blockSize / 3; ++i) {
                *block++ = patterns[round - 4][0];
                *block++ = patterns[round - 4][1];
                *block++ = patterns[round - 4][2];
            }
            break;
    }
}
