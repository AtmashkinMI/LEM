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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/resource.h>

#include <linux/oom.h>

#include "overwriters.hpp"
#include "memoryeraser.hpp"

MemoryEraser::MemoryEraser(int owrType)
{
    overwriter = intToOwr(owrType);

    block = (char *) malloc(ERASE_BLOCK_SIZE);
}

MemoryEraser::~MemoryEraser()
{
    if (overwriter) {
        delete overwriter;
    }

    if (block) {
        free(block);
    }
}

bool MemoryEraser::changeOOMScore()
{
    bool ret = true;

    if (FILE *oomScoreFile = fopen("/proc/self/oom_score_adj", "w")) {
        if (fprintf(oomScoreFile, "%d", OOM_SCORE_ADJ_MAX) < 0) {
            ret = false;
        }

        if (fclose(oomScoreFile) != 0) {
            ret = false;
        }
    }
    else {
        ret = false;
    }

    return ret;
}

bool MemoryEraser::needCheck(int round)
{
    return overwriter->verificationType() == Overwriter::AllRouns ||
           (overwriter->verificationType() == Overwriter::LastRound &&
            round == overwriter->getRounds() - 1);
}

int MemoryEraser::erase()
{
    if (!overwriter || !block) {
        return EXIT_ERROR;
    }

    const struct rlimit resLimits = {RLIM_INFINITY, RLIM_INFINITY};

    if (setrlimit(RLIMIT_AS, &resLimits)    ||
        setrlimit(RLIMIT_DATA, &resLimits)  ||
        setrlimit(RLIMIT_STACK, &resLimits) ||
        setrlimit(RLIMIT_RSS, &resLimits)   ||
        setrlimit(RLIMIT_MEMLOCK, &resLimits)) {
        return EXIT_ERROR;
    }

    if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
        return EXIT_ERROR;
    }

    if (!changeOOMScore()) {
        return EXIT_ERROR;
    }

    char *memPtr;

    long long bytesCount = 0;

    setlinebuf(stdout);

    for (int chunkSize = ERASE_BLOCK_SIZE; chunkSize > 0; --chunkSize) {
        while ((memPtr = (char *) malloc(chunkSize)) != 0) {
            for (int round = 0; round < overwriter->getRounds(); ++round) {
                overwriter->fillBlock(block, ERASE_BLOCK_SIZE, round);
                memcpy(memPtr, block, chunkSize);

                if (needCheck(round)) {
                    if (memcmp(memPtr, block, chunkSize) != 0) {
                        return EXIT_ERROR;
                    }
                }
            }

            bytesCount += chunkSize;
            printf("%lld\n", bytesCount);
        }
    }

    return EXIT_SUCCESS;
}
