#pragma once

#include <stdint.h>

extern void Sleep(unsigned time_ms);

namespace websocket
{
    void delay(uint32_t ms)
    {
        ::Sleep(ms);
    }
}
