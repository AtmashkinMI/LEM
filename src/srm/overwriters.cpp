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

#include "overwriters.hpp"

Overwriter *intToOwr(int owrType)
{
    switch (owrType) {
        case  0: return new ZeroOverwriter();
        case  1: return new OneOverwriter();
        case  2: return new RandomOverwriter();
        case  3: return new HMGBaseOverwriter();
        case  4: return new ISMOverwriter();
        case  5: return new GostOverwriter();
        case  6: return new HMGEnhancedOverwriter();
        case  7: return new USArmyOverwriter();
        case  8: return new USDoDOverwriter();
        case  9: return new USDoDEOverwriter();
        case 10: return new NAVSORLLOverwriter();
        case 11: return new NAVSOMFMOverwriter();
        case 12: return new AFSSIOverwriter();
        case 13: return new BSIOverwriter();
        case 14: return new USDoDECEOverwriter();
        case 15: return new RCMPOverwriter();
        case 16: return new VSITROverwriter();
        case 17: return new SchneierOverwriter();
        case 18: return new PfitznerOverwriter();
        case 19: return new GutmannOverwriter();
        default: return 0;
    }
}
