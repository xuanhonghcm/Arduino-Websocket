#pragma once

#include "ws_string.h"
#include <cstdio>


namespace websocket
{
    struct Stream
    {
    };

    inline Stream& operator<<(Stream& out, int i)
    {
        printf("%d", i);
        return out;
    }

    inline Stream& operator<<(Stream& out, char const* s)
    {
        puts(s);
        return out;
    }

    inline Stream& operator<<(Stream& out, String str)
    {
        char const* ptr = str.buffer;
        for(unsigned i = 0; i < str.len; i++)
        {
            putchar(*ptr);
            ptr++;
        }

        return out;
    }

    inline Stream& operator<<(Stream& out, char ch)
    {
        putchar(ch);
        return out;
    }

    // TODO: overload more operators as needed.

    static Stream debug;
}

#define debug_stream websocket::debug

