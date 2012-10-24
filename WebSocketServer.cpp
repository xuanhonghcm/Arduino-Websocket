//#define DEBUGGING

#include "WebSocketServer.h"

#include "sha1.h"
#include "base64.h"


bool WebSocketServer::handshake(Client &client) {

    socket_client = &client;

    // If there is a connected client->
    if (socket_client->connected()) {
        // Check request and look for websocket handshake
#ifdef DEBUGGING
            Serial.println(F("Client connected"));
#endif
        if (analyzeRequest(BUFFER_LENGTH)) {
#ifdef DEBUGGING
                Serial.println(F("Websocket established"));
#endif

                return true;

        } else {
            // Might just need to break until out of socket_client loop.
#ifdef DEBUGGING
            Serial.println(F("Disconnecting client"));
#endif
            disconnectStream();

            return false;
        }
    } else {
        return false;
    }
}

bool WebSocketServer::analyzeRequest(int bufferLength) {
    // Use String library to do some sort of read() magic here.
    String temp;

    int bite;
    bool foundupgrade = false;
    String oldkey[2];
    unsigned long intkey[2];
    String newkey;

#ifdef DEBUGGING
    Serial.println(F("Analyzing request headers"));
#endif

    // TODO: More robust string extraction
    while ((bite = socket_client->read()) != -1) {

        temp += (char)bite;

        if ((char)bite == '\n') {
#ifdef DEBUGGING
            Serial.print("Got Line: " + temp);
#endif
            // TODO: Should ignore case when comparing and allow 0-n whitespace after ':'. See the spec:
            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
            if (!foundupgrade && temp.startsWith("Upgrade: WebSocket")) {
                // OK, it's a websockets handshake for sure
                foundupgrade = true;
            } else if (!foundupgrade && temp.startsWith("Upgrade: websocket")) {
                foundupgrade = true;
            } else if (temp.startsWith("Origin: ")) {
                origin = temp.substring(8,temp.length() - 2); // Don't save last CR+LF
            } else if (temp.startsWith("Host: ")) {
                host = temp.substring(6,temp.length() - 2); // Don't save last CR+LF
            } else if (temp.startsWith("Sec-WebSocket-Key1: ")) {
                oldkey[0]=temp.substring(20,temp.length() - 2); // Don't save last CR+LF
            } else if (temp.startsWith("Sec-WebSocket-Key2: ")) {
                oldkey[1]=temp.substring(20,temp.length() - 2); // Don't save last CR+LF
            } else if (temp.startsWith("Sec-WebSocket-Key: ")) {
                newkey=temp.substring(19,temp.length() - 2); // Don't save last CR+LF
            }
            temp = "";		
        }

        if (!socket_client->available()) {
          delay(20);
        }
    }

    if (!socket_client->connected()) {
        return false;
    }

    temp += 0; // Terminate string

    // Assert that we have all headers that are needed. If so, go ahead and
    // send response headers.
    if (foundupgrade == true) {

        if (newkey.length() > 0) {

            Sha1 sha;
            sha.update(newkey);
            // add the magic string
            sha.update("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            uint8_t result[21];
            sha.finish(result);

            char b64Result[30];
            base64_encode(b64Result, reinterpret_cast<char*>(result), 20);

            socket_client->print(F("HTTP/1.1 101 Web Socket Protocol Handshake" CRLF
                                   "Upgrade: websocket" CRLF
                                   "Connection: Upgrade" CRLF
                                   "Sec-WebSocket-Accept: "));
            socket_client->print(b64Result);
            socket_client->print(CRLF CRLF);

            return true;
        } else {
            // something went horribly wrong
            return false;
        }
    } else {
        // Nope, failed handshake. Disconnect
#ifdef DEBUGGING
        Serial.println(F("Header mismatch"));
#endif
        return false;
    }
}

String WebSocketServer::handleStream() {
    uint8_t msgtype;
    uint8_t bite;
    unsigned int length;
    uint8_t mask[4];
    uint8_t index;
    unsigned int i;

    // String to hold bytes sent by client to server.
    String socketString;

    if (socket_client->connected() && socket_client->available()) {

        msgtype = timedRead();
        if (!socket_client->connected()) {
            return socketString;
        }

        length = timedRead() & 127;
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

        for (i=0; i<length; ++i) {
            socketString += (char) (timedRead() ^ mask[i % 4]);
            if (!socket_client->connected()) {
                return socketString;
            }
        }
    }

    return socketString;
}

void WebSocketServer::disconnectStream() {
#ifdef DEBUGGING
    Serial.println(F("Terminating socket"));
#endif

    // Should send 0x8700 to server to tell it I'm quitting here.
    socket_client->write((uint8_t) 0x87);
    socket_client->write((uint8_t) 0x00);
    
    socket_client->flush();
    delay(10);
    socket_client->stop();
}

String WebSocketServer::getData() {
    return handleStream();
}

void WebSocketServer::sendData(const char *str) {
#ifdef DEBUGGING
    Serial.print(F("Sending data: "));
    Serial.println(str);
#endif
    if (socket_client->connected()) {
        sendEncodedData(str);
    }
}

void WebSocketServer::sendData(String str) {
#ifdef DEBUGGING
    Serial.print(F("Sending data: "));
    Serial.println(str);
#endif
    if (socket_client->connected()) {
        sendEncodedData(str);
    }
}

int WebSocketServer::timedRead() {
  while (!socket_client->available()) {
    delay(20);  
  }

  return socket_client->read();
}

void WebSocketServer::sendEncodedData(char *str) {
    int size = strlen(str);

    // string type
    socket_client->write(0x81);

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

void WebSocketServer::sendEncodedData(String str) {
    int size = str.length() + 1;
    char cstr[size];

    str.toCharArray(cstr, size);

    sendEncodedData(cstr);
}
