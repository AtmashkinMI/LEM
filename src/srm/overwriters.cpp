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
