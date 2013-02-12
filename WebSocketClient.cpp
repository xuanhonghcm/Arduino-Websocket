#include "WebSocketClient.h"

#include "Sha1.h"
#include "base64.h"
#include "ws_debug.h"
#include "ws_string.h"
#include "ws_timer.h"
#include "ws_random.h"
#include "ws_progmem.h"

#define Assert(x) Assert_(#x, x)

static void Assert_(char const*, bool c)
{
    // TODO: write entire contents of the RAM to serial.
    while (!c) {}
}

using namespace websocket;
namespace ws = websocket;

ws::Result ws::clientHandshake(Socket& socket_, char const* host_, char const* path_)
{
    if (!socket_.connected())
    {
        return Error_NotConnected;
    }

    static bool seeded = false;
    if (!seeded)
    {
        ws::randomSeed();
        seeded = true;
    }

    uint8_t const keySize = 16;
    long keyStart[keySize/sizeof(long)+1] = {0};
    for (unsigned i = 0; i < countof(keyStart); ++i)
    {
        keyStart[i] = ws::random();
    }

    char b64Key[32] = {0};
    base64_encode(b64Key, reinterpret_cast<char*>(keyStart), keySize);

    wsDebug() << F("Sending websocket upgrade headers\n");

    socket_.print(F("GET "));
    socket_.print(path_);
    socket_.print(F(" HTTP/1.1" CRLF
                    "Upgrade: websocket" CRLF
                    "Connection: Upgrade" CRLF
                    "Host: "));
    socket_.print(host_);
    socket_.print(F(CRLF "Sec-WebSocket-Key: "));
    socket_.print(b64Key);
    socket_.print(F(CRLF "Sec-WebSocket-Version: 13" CRLF CRLF));

    wsDebug() << F("Analyzing response headers\n");

    while (socket_.connected() && !socket_.available())
    {
        ws::delay(100);
        wsDebug() << ".";
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

    // TODO: check the HTTP status code. abort if it's not 101 (Switching protocols).

    ws::String headerName;
    ws::String headerValue;
    ws::String serverKey;
    bool foundupgrade = false;
    while (state != end_of_headers)
    {
        int bite = socket_.read();
        if (bite == -1)
        {
            if (!socket_.connected())
            {
                return Error_Disconnected;
            }
            else
            {
                ws::delay(10);
                continue;
            }
        }

        switch (state)
        {
        case status_line:
            if ((char)bite == '\n')
            {
                state = header_name;
                headerName = ws::String();
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
                // Read the remainder of CRLF.
                socket_.read();

                state = end_of_headers;
            }
            else if ((char)bite != ':')
            {
                headerName += (char)bite;
            }
            else
            {
                state = header_sep;
                headerValue = ws::String();
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
                    wsDebug() << "accept found" << headerValue << "\n";
                    serverKey = headerValue;
                }
                state = end_of_header;
                headerName = ws::String();
            }
            break;

        }

        if (!socket_.available())
        {
          ws::delay(20);
        }
    }

    Sha1 sha;
    sha.update(reinterpret_cast<uint8_t*>(b64Key), strlen(b64Key));
    uint8_t const magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    sha.update(magic, sizeof(magic)-1);

    uint8_t result[21];
    sha.finish(result);

    char b64Result[30];
    base64_encode(b64Result, reinterpret_cast<char*>(result), 20);

    wsDebug()
        << F("  Server key:") << serverKey << "\n"
        << F("I calculated:") << b64Result << "\n";

    // if the keys match, good to go
    return serverKey.equals(ws::String(b64Result))
        ? Success_Ok
        : Error_BadHandshake;
}

WebSocket::WebSocket(Socket& socket)
    : socket_(socket)
    , framePtr_(0)
    , payloadLength_(0)
    , payloadRemainingBytes_(0)
{ }

Result WebSocket::readByte()
{
    if (!socket_.connected())
        return Error_NotConnected;

    int const readResult = socket_.read();
    if (readResult < 0)
        return Error_WouldBlock;

    messageBuffer_[framePtr_++] = static_cast<uint8_t>(readResult & 0xFF);
    return Success_Ok;
}

Result WebSocket::readFrame()
{
    while (true)
    {
        if (frameState_ != FrameState_Payload || payloadLength_ != 0)
        {
            Result const res = readByte();
            if (res != Success_Ok)
            {
                //Serial.print(res);Serial.print(" ");
                return error(res);
            }
        }


        switch (frameState_)
        {
        case FrameState_Opcode:
            Assert(framePtr_ == 1);
            //Serial.print("Opcode: "); Serial.println(messageBuffer_[0], HEX);
            if ((messageBuffer_[0] & Flag_Fin) != Flag_Fin)
            {
                //Serial.println("Unsupported frame type");
                // we only support single-frame messages. for now.
                return error(Error_UnsupportedFrameType);
            }
            frameState_ = FrameState_Length;
            break;

        case FrameState_Length:
            Assert(framePtr_ == 2);
            {
                //Serial.print("Length: "); Serial.println(messageBuffer_[1], HEX);
                uint8_t const len = messageBuffer_[1] & ~BitMask_Masked;
                switch (len)
                {
                default:
                    //Serial.println(len);
                    payloadRemainingBytes_ = len;
                    payloadLength_ = len;
                    if (messageBuffer_[1] & BitMask_Masked)
                        frameState_ = FrameState_Mask;
                    else
                        frameState_ = FrameState_Payload;
                    break;
                case 126: // following 2 bytes are the length
                    //Serial.println("2-byte length");
                    frameState_ = FrameState_Length2;
                    break;
                case 127: // following 8 bytes are the length
                    //Serial.println("8-byte length");
                    frameState_ = FrameState_Length8;
                    break;
                }
                break;
            }

        case FrameState_Length2:
            Assert(2 < framePtr_ && framePtr_ <= 4);
            if (framePtr_ == 4)
            {
                //Serial.println("Length2");
                uint16_t const len = messageBuffer_[2] << 8 | messageBuffer_[3];
                if (len > MaxFrameSize)
                    return error(Error_FrameTooBig);

                payloadRemainingBytes_ = len;
                payloadLength_ = len;

                if ((messageBuffer_[1] & BitMask_Masked) == BitMask_Masked)
                    frameState_ = FrameState_Mask;
                else
                    frameState_ = FrameState_Payload;
            }
            break;

        case FrameState_Length8:
            Assert(2 < framePtr_ && framePtr_ <= 10);
            if (framePtr_ == 10)
            {
                //Serial.println("Length8");
                // TODO: see what the real length is.
                return error(Error_FrameTooBig);
                if ((messageBuffer_[1] & BitMask_Masked) == BitMask_Masked)
                    frameState_ = FrameState_Mask;
                else
                    frameState_ = FrameState_Payload;
            }
            break;

        case FrameState_Mask:
            if (framePtr_ == 8 || framePtr_ == 14)
            {
                //Serial.println("Mask");
                frameState_ = FrameState_Payload;
            }
            break;

        case FrameState_Payload:
            //Serial.print("Payload"); Serial.println(payloadRemainingBytes_);
            if (--payloadRemainingBytes_ == 0)
            {
                Assert(framePtr_ > payloadLength_);

                uint8_t const* msg = messageBuffer_ + framePtr_ - payloadLength_;

                frameState_ = FrameState_Opcode;
                framePtr_ = 0;

                switch (messageBuffer_[0] & BitMask_Opcode)
                {
                default:
                    // unsupported frame type
                    return error(Error_UnsupportedFrameType);

                case Opcode_Text:
                    onTextFrame(reinterpret_cast<char const*>(msg), payloadLength_, messageBuffer_[0] & Flag_Fin);
                    break;

                case Opcode_Binary:
                    onBinaryFrame(msg, payloadLength_, messageBuffer_[0] & Flag_Fin);
                    break;

                case Opcode_Ping:
                    onPing(reinterpret_cast<char const*>(msg), payloadLength_);
                    break;

                case Opcode_Pong:
                    onPong(reinterpret_cast<char const*>(msg), payloadLength_);
                    break;

                case Opcode_Close:
                    disconnect();
                    return Error_Disconnected;
                }
            }
            break;
        }
    }

    Assert(false);
}

//Result WebSocket::getData(uint8_t* buffer, uint8_t bufferSize)
//{
//    // Read frames until error or final frame is received.
//    while (true)
//    {
//        uint8_t frameSize;
//        switch (Result const r = readFrame(buffer, bufferSize, frameSize))
//        {
//        default:
//            socket_.flush();
//            return r;

//        case Success_MoreFrames:
//            buffer += frameSize;
//            bufferSize -= frameSize;
//            break;
//        }
//    }
//}

Result WebSocket::sendData(char const* str, Opcode opcode)
{
    wsDebug() << F("Sending data: ") << str << "\n";

    return sendEncodedData(str, opcode);
}

void WebSocket::disconnect()
{
    wsDebug() << F("Terminating socket\n");

    if (socket_.connected())
    {
        uint8_t const msg[] = { Flag_Fin | Opcode_Close, 0x00 };
        socket_.write(msg, 2);
        socket_.flush();

        // Give the other end time to terminate the connection.
        ws::delay(10);

        socket_.stop();
    }
}

void WebSocket::dispatchEvents()
{
    // TODO: allow readFrame to perform only part of its job and continue later
    // For example when the frame is arriving slowly, get as much data as there
    // is available and allow other tasks to run.

    readFrame();
}


Result WebSocket::sendEncodedData(char const* str, Opcode opcode)
{
    if (!socket_.connected())
    {
        return error(Error_NotConnected);
    }
    else
    {
        // TODO: respect the maxFrameSize_
        // TODO: check that writes succeed.

        int const size = strlen(str);

        // string type
        socket_.write(uint8_t(Flag_Fin | opcode));

        // NOTE: no support for > 16-bit sized messages
        if (size > 125)
        {
            socket_.write(126);
            socket_.write((uint8_t) (size >> 8));
            socket_.write((uint8_t) (size && 0xFF));
        }
        else
        {
            socket_.write((uint8_t) size);
        }

        socket_.write(reinterpret_cast<uint8_t const*>(str), size);

        // XXX: Silently ignore failures
        return Success_Ok;
    }
}

Result WebSocket::error(Result err)
{
    Assert(err != Success_Ok);
    Assert(err != Success_MoreFrames);

    switch (err)
    {
    default:
        onError(err);
        disconnect();
        break;
    case Error_WouldBlock:
        break;

    case Error_Disconnected:
        onClose();
        break;
    }

    return err;
}
