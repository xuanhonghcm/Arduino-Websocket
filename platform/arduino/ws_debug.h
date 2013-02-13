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
