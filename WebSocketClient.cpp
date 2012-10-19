#define DEBUGGING

#include "WebSocketClient.h"

#include "global.h"
#include <Arduino.h>
#include <WString.h>
#include <Client.h>
//#include <Serial.h>
#include "Sha1.h"
#include "base64.h"

using namespace websocket;

bool WebSocketClient::handshake(Client &client) {

    socket_client = &client;

    // If there is a connected client->
    if (socket_client->connected()) {
        // Check request and look for websocket handshake
#ifdef DEBUGGING
            Serial.println(F("Client connected"));
#endif
        if (analyzeResponse()) {
#ifdef DEBUGGING
                Serial.println(F("Websocket established"));
#endif

                return true;

        } else {
            // Might just need to break until out of socket_client loop.
#ifdef DEBUGGING
            Serial.println(F("Invalid handshake"));
#endif
            disconnectStream();

            return false;
        }
    } else {
        return false;
    }
}

bool WebSocketClient::analyzeResponse() {
    String temp;

    int bite;
    bool foundupgrade = false;
    unsigned long intkey[2];
    String serverKey;
    char keyStart[17];
    char b64Key[25];
    String key = "------------------------";

    randomSeed(analogRead(0));

    for (int i=0; i<16; ++i) {
        keyStart[i] = (char)random(1, 256);
    }

    base64_encode(b64Key, keyStart, 16);

    for (int i=0; i<24; ++i) {
        key[i] = b64Key[i];
    }

#ifdef DEBUGGING
    Serial.println(F("Sending websocket upgrade headers"));
#endif    

    socket_client->print(F("GET "));
    socket_client->print(path);
    socket_client->print(F(" HTTP/1.1" CRLF
                           "Upgrade: websocket" CRLF
                           "Connection: Upgrade" CRLF
                           "Host: "));
    socket_client->print(host);
    socket_client->print(F(CRLF "Sec-WebSocket-Key: "));
    socket_client->print(key);
    socket_client->print(F(CRLF "Sec-WebSocket-Version: 13" CRLF CRLF));

#ifdef DEBUGGING
    Serial.println(F("Analyzing response headers"));
#endif    

    while (socket_client->connected() && !socket_client->available()) {
        delay(100);
#ifdef DEBUGGING
        Serial.print(".");
#endif
    }

    enum State
    {
        status_line,
        header_name,
        header_sep,
        header_value,
        end_of_header,
        end_of_headers
    };

    State state = status_line;


    String headerName;
    String headerValue;

    while (state != end_of_headers)
    {
        bite = socket_client->read();
        if (bite == -1)
        {
            if (!socket_client->connected())
            {
                // disconnected
                break;
            }
            else
            {
                delay(10);
                continue;
            }
        }

        switch (state)
        {
        case status_line:
            if ((char)bite == '\n')
            {
                state = header_name;
                headerName = String();
            }
            break;
        case end_of_header:
            if (bite != '\r')
            {
                state = header_name;
            }
            break;
        case header_name:
            if ((char)bite == '\r' || (char)bite == '\n')
            {
                state = end_of_headers;
            }
            else if ((char)bite != ':')
            {
                headerName += (char)bite;
            }
            else
            {
                state = header_sep;
                headerValue = String();
            }
            break;
        case header_sep:
            if ((char)bite == ' ')
                break;
            else
                state = header_value;
            // no break
        case header_value:
            if (bite != '\r' && bite != '\n')
            {
                headerValue += (char)bite;
            }
            if (bite == '\r')
            {
                if (headerName.equalsIgnoreCase("upgrade")
                        && headerValue.equalsIgnoreCase("websocket"))
                {
                    foundupgrade = true;
                }
                else if (headerName.equalsIgnoreCase("sec-websocket-accept"))
                {
                    Serial.print(F("accept found"));
                    Serial.println(headerValue);
                    serverKey = headerValue;
                }
                state = end_of_header;
                headerName = String();
            }
            break;

        }

        if (!socket_client->available())
        {
          delay(20);
        }
    }

    Sha1 sha;
    sha.update(key);
    sha.update("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

    uint8_t result[21];
    sha.finish(result);

    char b64Result[30];
    base64_encode(b64Result, reinterpret_cast<char*>(result), 20);

#ifdef DEBUGGING
    Serial.print(F("  Server key:"));
    Serial.println(serverKey);
    Serial.print(F("I calculated:"));
    Serial.println(String(b64Result));
#endif
    // if the keys match, good to go
    return serverKey.equals(String(b64Result));
}


String WebSocketClient::handleStream() {
    uint8_t msgtype;
    uint8_t bite;
    unsigned int length;
    uint8_t mask[4];
    uint8_t index;
    unsigned int i;
    bool hasMask = false;

    // String to hold bytes sent by server to client
    String socketString;

    if (socket_client->connected() && socket_client->available()) {

        msgtype = timedRead();
        if (!socket_client->connected()) {
            return socketString;
        }

        length = timedRead();

        if (length > 127) {
            hasMask = true;
            length = length & 127;
        }


        if (!socket_client->connected()) {
            return socketString;
        }

        index = 6;

        if (length == 126) {
            length = timedRead() << 8;
            if (!socket_client->connected()) {
                return socketString;
            }
            
            length |= timedRead();
            if (!socket_client->connected()) {
                return socketString;
            }

        } else if (length == 127) {
#ifdef DEBUGGING
            Serial.println(F("No support for over 16 bit sized messages"));
#endif
            while(1) {
                // halt, can't handle this case
            }
        }

        if (hasMask) {
            // get the mask
            mask[0] = timedRead();
            if (!socket_client->connected()) {
                return socketString;
            }

            mask[1] = timedRead();
            if (!socket_client->connected()) {

                return socketString;
            }

            mask[2] = timedRead();
            if (!socket_client->connected()) {
                return socketString;
            }

            mask[3] = timedRead();
            if (!socket_client->connected()) {
                return socketString;
            }
        }

        if (hasMask) {
            for (i=0; i<length; ++i) {
                socketString += (char) (timedRead() ^ mask[i % 4]);
                if (!socket_client->connected()) {
                    return socketString;
                }
            }
        } else {
            for (i=0; i<length; ++i) {
                socketString += (char) timedRead();
                if (!socket_client->connected()) {
                    return socketString;
                }
            }            
        }

    }

    return socketString;
}

void WebSocketClient::disconnectStream() {
#ifdef DEBUGGING
    Serial.println(F("Terminating socket"));
#endif
    // Should send 0x8700 to server to tell it I'm quitting here.
    socket_client->write((uint8_t) 0x87); // Flag_Fin | Opcode_? Close?
    socket_client->write((uint8_t) 0x00); // no masking, zero length
    
    socket_client->flush();
    delay(10);
    socket_client->stop();
}

String WebSocketClient::getData() {
    String data;

    data = handleStream();

    return data;
}

void WebSocketClient::sendData(char const* str, Opcode opcode)
{
#ifdef DEBUGGING
    Serial.print(F("Sending data: "));
    Serial.println(str);
#endif
    if (socket_client->connected())
    {
        sendEncodedData(str, opcode);
    }
}

void WebSocketClient::sendData(String const& str)
{
#ifdef DEBUGGING
    Serial.print(F("Sending data: "));
    Serial.println(str);
#endif
    if (socket_client->connected())
    {
        sendEncodedData(str);
    }
}

int WebSocketClient::timedRead() {
  while (!socket_client->available()) {
    delay(20);  
  }

  return socket_client->read();
}

void WebSocketClient::sendEncodedData(char const* str, Opcode opcode)
{
    int const size = strlen(str);

    // string type
    socket_client->write(uint8_t(Flag_Fin | opcode));

    // NOTE: no support for > 16-bit sized messages
    if (size > 125) {
        socket_client->write(126);
        socket_client->write((uint8_t) (size >> 8));
        socket_client->write((uint8_t) (size && 0xFF));
    } else {
        socket_client->write((uint8_t) size);
    }

    for (int i=0; i<size; ++i) {
        socket_client->write(str[i]);
    }
}

void WebSocketClient::sendEncodedData(String const& str)
{
    // XXX: String should have c_str() or similar member.
    struct X : String { char const* c_str() const { return buffer; } };

    char const* s = static_cast<X const&>(str).c_str();
    sendEncodedData(s, Opcode_Text);
}
