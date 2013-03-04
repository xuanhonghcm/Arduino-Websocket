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

#include "WString.h"
#include "ws_debug.h"

namespace websocket
{
    class String : public ::String
    {
    public:
        String(char const* s = "") : ::String(s)
        { }

        char const* c_str() const
        {
            return buffer;
        }

        String& operator=(char const* rhs)
        {
            ::String::operator=(rhs);
            return *this;
        }

        String& operator=(::String const& rhs)
        {
            ::String::operator=(rhs);
            return *this;
        }

        String& operator=(String const& rhs)
        {
            ::String::operator=(rhs);
            return *this;
        }

        friend Stream& operator<<(Stream& out, websocket::String const& s)
        {
            return out << s.c_str();
        }

    };

} // namespace websocket

