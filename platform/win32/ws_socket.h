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

#include <Winsock2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <exception>

namespace websocket
{
    class Socket
    {
    private:
        // No copying!
        Socket(Socket const&);
        void operator=(Socket const&);


        class WinSockHelper
        {
        public:
            WinSockHelper()
            {
                WSADATA wsaData;
                if (::WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
                {
                    printf("Failed to initialize winsock\n");
                    throw std::exception();//"Failed to initialize winsocket");
                }
            }

            ~WinSockHelper()
            {
                ::WSACleanup();
            }
        };
    public:

        Socket()
            : socket_(INVALID_SOCKET)
        {
            static WinSockHelper helper;
        }

        ~Socket()
        {
            stop();
        }

        bool connect(char const* host, uint16_t port)
        {
            stop();

            sockaddr_in remoteAddr = {0};

            remoteAddr.sin_family = AF_INET;
            remoteAddr.sin_addr.s_addr = inet_addr(host);
            remoteAddr.sin_port = htons(port);

            SOCKET const s = ::socket(AF_INET, SOCK_STREAM, 0);
            if (s == INVALID_SOCKET)
                return false;

            int const res = ::connect(s, reinterpret_cast<sockaddr*>(&remoteAddr), sizeof(sockaddr_in));
            if (res == SOCKET_ERROR)
            {
                //printf("Connection to host %s failed, res=%d, last error #%d\n", host, res, WSAGetLastError());
                ::closesocket(s);
                return false;
            }
            else
            {
                event_ = WSACreateEvent();
                WSAEventSelect(s, event_, FD_READ | FD_CLOSE);
                socket_ = s;
                return true;
            }
        }

        void stop()
        {
            ::shutdown(socket_, SD_BOTH);
            ::closesocket(socket_);
            WSACloseEvent(event_);
            socket_ = INVALID_SOCKET;
        }

        bool connected() const
        {
            char ch;
            int const result = ::recv(socket_, &ch, 1, MSG_PEEK);
            switch (result)
            {
            default:
                // This should not happen. If it does, the socket is probably messed up anyway.
                return false;

            case 1:
                // data available -> still connected
                return true;

            case 0:
                // socket was gracefully closed
                return false;

            case SOCKET_ERROR:
                switch (::WSAGetLastError())
                {
                default:
                    // other error -> not connected
                    return false;

                case WSAEWOULDBLOCK:
                    // data not available, but still connected
                    return true;
                }
            }
        }

        bool available() const
        {
            char ch;
            int const result = ::recv(socket_, &ch, 1, MSG_PEEK);
            return result > 0;
        }

        /// Write a zero-terminated text to the socket. The terminating zero is not written.
        int print(char const* msg)
        {
            int const len = ::strlen(msg);
            return ::send(socket_, msg, len, 0);
        }

        int print(int n)
        {
            char buf[16];
            sprintf(buf, "%d", n);
            return print(buf);
        }

        /// Write a single byte to the socket.
        int write(uint8_t data)
        {
            return ::send(socket_, reinterpret_cast<char*>(&data), 1, 0);
        }

        /// Write \a count bytes to the socket.
        int write(uint8_t const* data, size_t count)
        {
            return ::send(socket_, reinterpret_cast<char const*>(data), count, 0);
        }

        /// @return the byte that was read from the socket.
        /// @retval -1 if failed
        int read()
        {
            uint8_t data;
            int const res = ::recv(socket_, reinterpret_cast<char*>(&data), 1, 0);
            if (res == 1)
                return data;
            else
                return -1;
        }

        int read(uint8_t* buf, size_t size)
        {
            return ::recv(socket_, reinterpret_cast<char*>(buf), size, 0);
        }

        void flush()
        {
            // nop
        }


    private:

        SOCKET socket_;
        HANDLE event_;
    };
}
