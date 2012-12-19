#pragma once

#include <stdlib.h>
#include <time.h>

namespace websocket
{
    inline void randomSeed()
    {
        int x;
        srand(time(0) ^ reinterpret_cast<int>(&x));
    }

    uint8_t random()
    {
        return ::rand() & 0xFF;
    }
}
