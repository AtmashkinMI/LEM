#ifndef OVERWRITERS_HPP
#define OVERWRITERS_HPP

#include "owr/zerooverwriter.hpp"
#include "owr/oneoverwriter.hpp"
#include "owr/randomoverwriter.hpp"
#include "owr/hmgbaseoverwriter.hpp"
#include "owr/ismoverwriter.hpp"
#include "owr/gostoverwriter.hpp"
#include "owr/hmgenhancedoverwriter.hpp"
#include "owr/usarmyoverwriter.hpp"
#include "owr/usdodoverwriter.hpp"
#include "owr/usdodeoverwriter.hpp"
#include "owr/navsorlloverwriter.hpp"
#include "owr/navsomfmoverwriter.hpp"
#include "owr/afssioverwriter.hpp"
#include "owr/bsioverwriter.hpp"
#include "owr/usdodeceoverwriter.hpp"
#include "owr/rcmpoverwriter.hpp"
#include "owr/vsitroverwriter.hpp"
#include "owr/schneieroverwriter.hpp"
#include "owr/pfitzneroverwriter.hpp"
#include "owr/gutmannoverwriter.hpp"

Overwriter *intToOwr(int owrType);

#endif // OVERWRITERS_HPP
