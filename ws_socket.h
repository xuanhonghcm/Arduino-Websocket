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

// Define HAVE_LIBWEBSOCKET_SOCKET if you want to provide your own implementation
// of this websocket::Socket interface.
#if defined(HAVE_LIBWEBSOCKET_SOCKET)
    // externally provided implementation must implement this interface.

    #include <cassert>
    #include <cstddef>
    #include <cstdint>
    #include <cstring>

    namespace websocket
    {
        class Socket
        {
        public:
            virtual ~Socket() { }

            /// @retval true if socket is connected.
            virtual bool connected() const = 0;

            /// @retval true    Calling read won't block.
            /// @retval false   No data available, calling read may block.
            virtual bool available() const = 0;

            /// @retval -1             If reading failed
            /// @retval non-negative   A byte that was read from the socket.
            virtual int read() = 0;

            /// Write a buffer to socket.
            /// @param data   Non-null pointer to a buffer.
            /// @param count  Number of bytes in buffer.
            /// @return       Number of bytes written.
            /// @retval -1    If writing failed.
            virtual int write(uint8_t const* data, size_t count) = 0;

            /// Forces data to be sent over the wire.
            virtual void flush() = 0;

            /// Shut down communication and close the socket.
            /// @post connected() returns false.
            virtual void stop() = 0;

            int write(uint8_t data)
            {
                return write(&data, 1);
            }

            /// Write a null-terminated string to socket.
            /// @pre        s is not null
            /// @see write  Return value has same meaning.
            int print(char const* s)
            {
                assert(s != nullptr);
                return write(reinterpret_cast<uint8_t const*>(s), strlen(s));
            }
        };
    }

#endif

#if !defined(HAVE_LIBWEBSOCKET_SOCKET) && defined(ARDUINO)
    #define HAVE_LIBWEBSOCKET_SOCKET
    #include "platform/arduino/ws_socket.h"
#endif

#if !defined(HAVE_LIBWEBSOCKET_SOCKET) && defined(_WIN32)
    #define HAVE_LIBWEBSOCKET_SOCKET
    #include "platform/win32/ws_socket.h"
#endif
