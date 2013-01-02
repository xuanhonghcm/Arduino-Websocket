#pragma once

#include <Windows.h>
#include <stdint.h>

namespace websocket
{
    void delay(uint32_t ms)
    {
        ::Sleep(ms);
    }
}
