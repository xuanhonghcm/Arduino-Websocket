#include "ws_socket.h"

namespace websocket
{
    bool Socket::connect(char const* host, uint16_t port)
    {
        uint8_t rawIp[4];
        const uint32_t ip = ::inet_addr(host);
        rawIp[0] = (ip>>24) & 0xFF;
        rawIp[1] = (ip>>16) & 0xFF;
        rawIp[2] = (ip>>8) & 0xFF;
        rawIp[3] = ip & 0xFF;

        int8_t s = ::socket(0, IPPROTO_TCP);

        if (s < 0)
            return false;

        socket_ = s;

        int const res = ::connect(socket_, rawIp, port);
        if (res == 0)
        {
            //printf("Connection to host %s failed, res=%d, last error #%d\n", host, res, WSAGetLastError());
            stop();
            return false;
        }
        else
        {
            return true;
        }
    }

    void Socket::stop()
    {
        ::close(socket_);
        socket_ = 0xFF;
    }

    bool Socket::connected() const
    {
        uint8_t ch;
        int const result = ::recv(socket_, &ch, 1, MSG_PEEK);
        switch (result)
        {
            default:
            // This should not happen. If it does, the socket is probably messed up anyway.
            return false;

            case 1:
            // data available -> still connected
            return true;

            case -1:
            // data not available, but still connected
            return true;

            case 0:
            // socket was gracefully closed
            return false;
        }
    }

    bool Socket::available() const
    {
        uint8_t ch;
        int const result = ::recv(socket_, &ch, 1, MSG_PEEK);
        return result > 0;
    }

    /// Write a zero-terminated text to the socket. The terminating zero is not written.
    int Socket::print(char const* msg)
    {
        int const len = strlen(msg);
        return ::send(socket_, reinterpret_cast<const uint8_t*>(msg), len);
    }

    int Socket::print(int n)
    {
        char buf[16];
        sprintf(buf, "%d", n);
        return print(buf);
    }

    /// Write a single byte to the socket.
    int Socket::write(uint8_t data)
    {
        return ::send(socket_, reinterpret_cast<const uint8_t*>(&data), 1);
    }

    /// Write \a count bytes to the socket.
    int Socket::write(uint8_t const* data, size_t count)
    {
        return ::send(socket_, reinterpret_cast<const uint8_t*>(data), count);
    }

    /// @return the byte that was read from the socket.
    /// @retval -1 if failed
    int Socket::read()
    {
        uint8_t data;
        int const res = ::recv(socket_, &data, 1, 0);

        return (res == 1) ? data : -1;
    }

    int Socket::read(uint8_t* buf, size_t size)
    {
        return ::recv(socket_, buf, size, 0);
    }

    void Socket::flush()
    {
        // nop
    }

}   //  namespace websocket
