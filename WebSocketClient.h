/*
Websocket-Arduino, a websocket implementation for Arduino
Copyright 2011 Per Ejeklint

Based on previous implementations by
Copyright 2010 Ben Swanson
and
Copyright 2010 Randall Brewer
and
Copyright 2010 Oliver Smith

Some code and concept based off of Webduino library
Copyright 2009 Ben Combee, Ran Talbott

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

-------------
Now based off
http://tools.ietf.org/html/rfc6455

*/


#ifndef WEBSOCKETCLIENT_H_
#define WEBSOCKETCLIENT_H_

#include <stdint.h>

// CRLF characters to terminate lines/handshakes in headers.
#define CRLF "\r\n"

class Client;
class String;

namespace websocket {

template <typename T, unsigned count>
char (&countof_impl(T(&)[count]))[count];

#define countof(x) (sizeof(::websocket::countof_impl(x)))


enum Opcode
{
    Opcode_Continuation = 0x00,
    Opcode_Text         = 0x01,
    Opcode_Binary       = 0x02,
    Opcode_Close        = 0x08,
    Opcode_Ping         = 0x09,
    Opcode_Pong         = 0x0A,
};

enum Flag
{
    Flag_Fin = 0x80,
    Flag_Rsv1 = 0x40,
    Flag_Rsv2 = 0x20,
    Flag_Rsv3 = 0x10,
};

enum Result
{
    Success_Ok,
    Success_MoreFrames,
    /// Returned when WebSocketClient is ni invalid state for the operation.
    Error_InvalidState,
    /// Returned when socket_client is not connected when operation requires connection.
    Error_NotConnected,
    /// Returned when connection is closed during operation.
    Error_Disconnected,
    /// Returned when frame received from server is larger than this lib can handle.
    Error_FrameTooBig,
    /// Returned when provided buffer is not sufficient.
    Error_InsufficientBuffer,
    /// Returned when operation times out.
    Error_Timeout,
    /// Server returned invalid value to handshake
    Error_BadHandshake

};

typedef ::Client Socket;

class ClientHandshake
{
public:

    ClientHandshake(Socket& socket, char const* host, char const* path);

    Result run();

private:

    /// Performs the handshake.
    Result analyzeResponse();

    /// Sends "close" frame to server and closes the socket.
    void disconnect();

    Socket& socket_;
    char const* const path_;
    char const* const host_;
};

class WebSocket
{
public:

    explicit WebSocket(Socket& socket, uint8_t maxFrameSize = 64);

    // Get data off of the stream
    Result getData(uint8_t* buffer, uint8_t bufferSize);
    Result readFrame(uint8_t *buffer, uint8_t bufferSize, uint8_t& frameSize);

    // Write data to the stream
    void sendData(char const* str, Opcode = Opcode_Text);
    void sendData(String const& str);

private:

    Socket& socket_;
    uint8_t const maxFrameSize_;

    int timedRead();

    void sendEncodedData(char const* str, Opcode opcode);
    void sendEncodedData(String const& str);
};

} // namespace websocket

#endif
