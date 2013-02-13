#pragma once

extern "C" unsigned long hardclock();   // returns random value from true random number generator

namespace websocket
{
    inline void randomSeed()
    {
        // nop
    }

    uint8_t random()
    {
        return ::hardclock() & 0xFF;
    }
}
