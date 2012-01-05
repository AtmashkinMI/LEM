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

#ifndef NAVSOMFMOVERWRITER_HPP
#define NAVSOMFMOVERWRITER_HPP

#include <stdio.h>

#include "overwriter.hpp"

class NAVSOMFMOverwriter : public Overwriter
{
public:
    NAVSOMFMOverwriter();

    int getRounds();
    bool isConstantRound(int round);
    void fillBlock(char *block, int blockSize, int round);

    QString getName();
    QString getDescription();

    Verification verificationType();

    ~NAVSOMFMOverwriter();

private:
    FILE *randomSource;

    static const char pattern[4];
};

#endif // NAVSOMFMOVERWRITER_HPP
