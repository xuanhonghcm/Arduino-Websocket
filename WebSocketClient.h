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

/*
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

/// Performs the client part of the handshake.
Result clientHandshake(Socket& socket, char const* host, char const* path);

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
