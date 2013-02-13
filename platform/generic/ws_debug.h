/*
    Arduino-Websocket, a websocket implementation for Arduino
    Copyright 2012, 2013 Ahti Legonkov, Raido Lepp

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

