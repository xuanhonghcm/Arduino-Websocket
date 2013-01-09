#pragma once

#include <Arduino.h>
#include "ws_progmem.h"

namespace websocket
{
    struct Stream
    {
    };

    inline Stream& operator<<(Stream& out, int i)
    {
        Serial.print(i);
        return out;
    }

    inline Stream& operator<<(Stream& out, char const* s)
    {
        Serial.print(s);
        return out;
    }

    inline Stream& operator<<(Stream& out, char ch)
    {
        Serial.print(ch);
        return out;
    }

    inline Stream& operator<<(Stream& out, __FlashStringHelper const* s)
    {
        PROGRAM_MEMORY(const char)* p = reinterpret_cast<PROGRAM_MEMORY(const char)*>(s);
        while (unsigned char c = readByteFromProgramMemory(p++))
        {
            out << c;
        }
        return out;
    }

    // TODO: overload more operators as needed.

    static Stream debug;
}

#define debug_stream websocket::debug
