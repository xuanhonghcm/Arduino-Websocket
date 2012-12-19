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

#include "ws_socket.h"
#include <stdint.h>

// CRLF characters to terminate lines/handshakes in headers.
#define CRLF "\r\n"

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
    Opcode_Pong         = 0x0A
};

enum Flag
{
    Flag_Fin = 0x80,
    Flag_Rsv1 = 0x40,
    Flag_Rsv2 = 0x20,
    Flag_Rsv3 = 0x10,
};

enum BitMask
{
    BitMask_Opcode = 0x0F,
    BitMask_Flags  = 0xF0,
    BitMask_Masked = 0x80,
    BitMask_Length = 0x7F
};

enum Result
{
    Success_Ok,
    Success_MoreFrames,
    Error_WouldBlock,
    Error_ReadFailed,
    /// Returned when WebSocketClient is ni invalid state for the operation.
    Error_InvalidState,
    /// Returned when socket_client is not connected when operation requires connection.
    Error_NotConnected,
    /// Returned when connection is closed during operation.
    Error_Disconnected,
    /// Returned when frame received from server is larger than this lib can handle.
    Error_FrameTooBig,
    Error_UnsupportedFrameType,
    /// Returned when provided buffer is not sufficient.
    Error_InsufficientBuffer,
    /// Returned when operation times out.
    Error_Timeout,
    /// Server returned invalid value to handshake
    Error_BadHandshake
};

class ClientHandshake
{
public:

    ClientHandshake(Socket& socket, char const* host, char const* path);

    /// Performs the handshake.
    Result run();

private:

    Socket& socket_;
    char const* const path_;
    char const* const host_;
};

class WebSocket
{
public:

    static uint8_t const MaxFrameSize = 64;
    static uint16_t const MaxMessageSize = 64;
    explicit WebSocket(Socket& socket);

    // Write data to the stream
    Result sendData(char const* str, Opcode = Opcode_Text);

    /// Disconnects the websocket. Before closing the socket, sends a "Close" control frame.
    /// After closing the connection generates onClose event.
    void disconnect();

    /// Sends a "Ping" control frame.
    void ping();

    /// Sends a "Pong" control frame.
    void pong();

    /// This function should be called in loop() function.
    void dispatchEvents();

protected:

    /// This function is called by websocket when the socket is closed. This could be
    /// either by server or client.
    virtual void onClose() {}

    /// This function is called by websocket when an error has been detected.
    virtual void onError(Result err) { }

    /// This function is called by websocket when a text message is received.
    virtual void onTextMessage(char const* msg, uint16_t size) {}

    /// This function is called by websocket after a single text frame is received.
    virtual void onTextFrame(char const* msg, uint16_t size, bool isLast) {}

    /// This function is called by websocket after a whole binary message is received.
    virtual void onBinaryMessage(uint8_t const* msg, uint16_t size) {}

    /// This function is called by websocket after a single binary frame is received.
    virtual void onBinaryFrame(uint8_t const* msg, uint8_t size, bool isLast) {}

    /// This function is called by websocket when a ping frame is received.
    virtual void onPing(char const* payload, uint8_t size) {}

    /// This function is called by websocket when a pong frame is received.
    virtual void onPong(char const* payload, uint8_t size) {}

private:

    Result sendEncodedData(char const* str, Opcode opcode);

    // Get data off of the stream
    //Result getData(uint8_t* buffer, uint8_t bufferSize);
    Result readFrame();
    Result readByte();
    Result error(Result);

private:

    Socket& socket_;
    enum FrameState
    {
        FrameState_Opcode,
        FrameState_Length,
        FrameState_Length2,
        FrameState_Length8,
        FrameState_Mask,
        FrameState_Payload
    };
    FrameState frameState_;

    /// Index into frameBuffer_ where next byte of payload is to be written.
    uint8_t framePtr_;
    uint8_t payloadLength_;
    uint8_t payloadRemainingBytes_;
    uint8_t messageBuffer_[MaxMessageSize];

};

} // namespace websocket

#endif
