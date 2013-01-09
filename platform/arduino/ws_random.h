#pragma once

#include <Arduino.h>
#include <limits.h>

namespace websocket
{
    inline void randomSeed()
    {
        ::randomSeed(analogRead(0));
    }

    inline long random()
    {
        return ::random(LONG_MIN, LONG_MAX);
    }
}
