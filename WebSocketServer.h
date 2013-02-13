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
http://www.whatwg.org/specs/web-socket-protocol/
*/


#ifndef WEBSOCKETSERVER_H_
#define WEBSOCKETSERVER_H_

#include <Arduino.h>
#include <Client.h>
#include "ws_string.h"

// CRLF characters to terminate lines/handshakes in headers.
#define CRLF "\r\n"

class Client;

namespace websocket
{
typedef ::Client Socket;

class ServerHandshake
{
public:

    /// @param socket - a connected socket
    /// @param acceptOrigin - origin that the connctions should be accepted from.
    ///                       if empty, then any origin will be accepted.
    ServerHandshake(Socket& socket, String const& acceptOrigin);

    // Perform server part of websocket handshake
    bool run();

private:
    // Discovers if the client's header is requesting an upgrade to a
    // websocket connection.
    bool analyzeRequest();

    // Disconnect user gracefully.
    void disconnect();

private:

    Socket& socket_;
    String acceptOrigin_;
    String origin_;
    String host_;
};

}

#endif
