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
http://www.whatwg.org/specs/web-socket-protocol/

- OLD -
Currently based off of "The Web Socket protocol" draft (v 75):
http://tools.ietf.org/html/draft-hixie-thewebsocketprotocol-75
*/


#ifndef WEBSOCKETCLIENT_H_
#define WEBSOCKETCLIENT_H_

#include <stdint.h>

// CRLF characters to terminate lines/handshakes in headers.
#define CRLF "\r\n"

// Amount of time (in ms) a user may be connected before getting disconnected 
// for timing out (i.e. not sending any data to the server).
#define TIMEOUT_IN_MS 10000

// ACTION_SPACE is how many actions are allowed in a program. Defaults to 
// 5 unless overwritten by user.
#ifndef CALLBACK_FUNCTIONS
#define CALLBACK_FUNCTIONS 1
#endif

// Don't allow the client to send big frames of data. This will flood the Arduinos
// memory and might even crash it.
#ifndef MAX_FRAME_LENGTH
#define MAX_FRAME_LENGTH 256
#endif

class Client;
class String;

namespace websocket {

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

class WebSocketClient {
public:


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
        Error_Timeout
    };

    // Handle connection requests to validate and process/refuse
    // connections.
    bool handshake(Client &client);
    
    // Get data off of the stream
    String getData();
    Result readFrame(uint8_t *buffer, uint8_t bufferSize);

    // Write data to the stream
    void sendData(char const* str, Opcode = Opcode_Text);
    void sendData(String const& str);

    char *path;
    char *host;

private:
    Client *socket_client;
    unsigned long _startMillis;

    const char *socket_urlPrefix;

    // Discovers if the client's header is requesting an upgrade to a
    // websocket connection.
    bool analyzeResponse();


    // Disconnect user gracefully.
    void disconnectStream();
    
    int timedRead();

    void sendEncodedData(char const* str, Opcode opcode);
    void sendEncodedData(String const& str);
};

} // namespace websocket

#endif
