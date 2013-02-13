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

#if defined(HAVE_LIBWEBSOCKET_RANDOM)
    namespace websocket
    {
        void randomSeed();
        uint32_t random();
    }
#endif

#if !defined(HAVE_LIBWEBSOCKET_RANDOM) && defined(ARDUINO)
    #define HAVE_LIBWEBSOCKET_RANDOM
    #include "platform/arduino/ws_random.h"
#endif

#if !defined(HAVE_LIBWEBSOCKET_RANDOM)
    #define HAVE_LIBWEBSOCKET_RANDOM
    #include "platform/generic/ws_random.h"
#endif
