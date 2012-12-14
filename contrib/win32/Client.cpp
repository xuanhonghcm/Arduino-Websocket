#include "client.h"
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <assert.h>

Client::Client()
    : m_isConnected(false)
{

}

Client::~Client()
{

}

int Client::connect(const char *host, uint16_t port)
{
    SOCKADDR_IN RemoteAddr;
    memset(&RemoteAddr, 0, sizeof(RemoteAddr));

    RemoteAddr.sin_family = AF_INET;
    RemoteAddr.sin_addr.s_addr = inet_addr(host);
    RemoteAddr.sin_port = htons(port);

    if (WSAStartup(MAKEWORD(2, 0), &m_wsaData))
    {
        return 0;
    }

    m_sClient = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sClient == INVALID_SOCKET)
    {
        WSACleanup();
        return 0;
    }

    int res = ::connect(m_sClient, (PSOCKADDR)&RemoteAddr, sizeof(SOCKADDR));

    if (res == 0)
    {
        m_isConnected = true;
    }
    else
    {
        //printf("Connection to host %s failed, res=%d, last error #%d\n", host, res, WSAGetLastError());
        closesocket(m_sClient);
        WSACleanup();
        return 0;
    }

    return 1;
}

size_t Client::write(uint8_t c)
{
    return ::send(m_sClient, reinterpret_cast<const char*>(&c), 1, 0);
}

size_t Client::write(const uint8_t *buf, size_t size)
{
    return ::send(m_sClient, reinterpret_cast<const char*>(buf), size, 0);
}

size_t Client::print(const char str[])
{
    return write(reinterpret_cast<uint8_t const*>(str), strlen(str));
}

size_t Client::print(uint8_t i)
{
    return write(i);
}

size_t Client::print(size_t i)
{
    return write(i);
}

void Client::flush()
{
    while (available())
    {
        read();
    }
}

void Client::stop()
{
    ::closesocket(m_sClient);
    //WSACleanup();
}

uint8_t Client::connected()
{
    return (m_isConnected) ? 1 : 0;
}

int Client::available()
{
    u_long result = 0;
    int res = ::ioctlsocket(m_sClient, FIONREAD, &result);

    if ( res )
    {
        //Serial.print("Socket error. Error #"); Serial.println(WSAGetLastError());
        return 0;
    }

    return result;
}

int Client::read(uint8_t *buf, size_t size)
{
    assert(buf);
    assert(size>0);

    if (!available())
        return -1;

    return ::recv(m_sClient, reinterpret_cast<char*>(buf), size, 0);
}

int Client::read()
{
    uint8_t c;

    if (!available())
        return -1;

    if (::recv(m_sClient, (char*)&c, 1, 0) > 0)
    {
        return c;
    }
    else
    {
        return -1;
    }
}
