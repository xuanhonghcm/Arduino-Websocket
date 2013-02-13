#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t SOCKET;

extern "C" {
    int8_t socket(int familyNotUsed, uint8_t protocol); // Opens a socket(TCP or UDP or IP_RAW mode)
    void close(SOCKET s); // Close socket
    uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port); // Establish TCP connection (Active connection)
    void disconnect(SOCKET s); // disconnect the connection
    uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len); // Send data (TCP)
    int16_t recv(SOCKET s, uint8_t * buf, int16_t len, int8_t isPeek);	// Receive data (TCP)
    uint32_t inet_addr(const char* host);
}

namespace websocket
{
    #define IPPROTO_TCP    1    //  value must apply to socket() implementation
    #define MSG_PEEK       1    //

    class Socket
    {
        private:
        // No copying!
        Socket(Socket const&);
        void operator=(Socket const&);

        public:

        Socket()
            : socket_(0xFF)
        {   }

        ~Socket()
        {   }

        bool connect(char const* host, uint16_t port);
        void stop();
        bool connected() const;

        bool available() const;

        /// Write a zero-terminated text to the socket. The terminating zero is not written.
        int print(char const* msg);
        int print(int n);

        /// Write a single byte to the socket.
        int write(uint8_t data);

        /// Write \a count bytes to the socket.
        int write(uint8_t const* data, size_t count);

        /// @return the byte that was read from the socket.
        /// @retval -1 if failed
        int read();

        int read(uint8_t* buf, size_t size);

        void flush();

        private:
        SOCKET socket_;
    };

}   //  namespace websocket
