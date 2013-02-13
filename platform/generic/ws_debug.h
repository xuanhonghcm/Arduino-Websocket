#pragma once

#include <cstdio>

namespace websocket
{
    class Stream
    {
    public:

        explicit Stream(FILE* f) : f_(f) { }

    private:

        FILE* f_;
        friend Stream& operator<<(Stream& out, char const* s);
        friend Stream& operator<<(Stream& out, int);
    };

    inline Stream& operator<<(Stream& out, char const* s)
    {
        fprintf(out.f_, "%s", s);
        return out;
    }

    inline Stream& operator<<(Stream& out, int i)
    {
        fprintf(out.f_, "%d", i);
        return out;
    }

    static Stream cerr(stderr);
}

#define debug_stream websocket::cerr

