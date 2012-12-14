#ifndef CLIENT_H
#define CLIENT_H

#include <stddef.h>
#include <stdint.h>
#include <winsock2.h>

// TODO: Copy-pase from Arduino class. Implementing it for Win32
class Client
{   
public:
    Client();
    ~Client();
//    virtual int connect(IPAddress ip, uint16_t port) =0;
    int connect(const char *host, uint16_t port);
    size_t print(const char str[]);
    size_t println();
    size_t print(uint8_t i);
    size_t print(size_t i);
    size_t write(uint8_t);
    size_t write(const uint8_t *buf, size_t size);
    int available();
    int read();
    int read(uint8_t *buf, size_t size);
    int peek();
    void flush();
    void stop();
    uint8_t connected();
    operator bool();
private:
    bool m_isConnected;
    WSADATA m_wsaData;
    SOCKET m_sClient;
protected:
    //uint8_t* rawIPAddress(IPAddress& addr) { return addr.raw_address(); };
};

#endif // CLIENT_H
