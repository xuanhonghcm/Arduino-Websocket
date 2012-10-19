#ifndef Sha1_h
#define Sha1_h

#include <inttypes.h>
#include <WString.h>

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

union _buffer {
  uint8_t b[BLOCK_LENGTH];
  uint32_t w[BLOCK_LENGTH/4];
};
union _state {
  uint8_t b[HASH_LENGTH];
  uint32_t w[HASH_LENGTH/4];
};

class Sha1
{
  public:
    Sha1();
    void update(uint8_t const* data, uint16_t len)
    {
        for (uint16_t i = 0; i < len; ++i)
        {
            write(data[i]);
        }
    }

    void update(uint8_t data)
    {
        write(data);
    }

    void update(String const& s)
    {
        for (uint16_t i = 0; i < s.length(); ++i)
        {
            write(s[i]);
        }
    }

    void finish(uint8_t* out);
  private:
    size_t write(uint8_t data);
    void pad();
    void addUncounted(uint8_t data);
    void hashBlock();
    _buffer buffer;
    uint8_t bufferOffset;
    _state state;
    uint32_t byteCount;
    uint8_t keyBuffer[BLOCK_LENGTH];
    uint8_t innerHash[HASH_LENGTH];
    
};

#endif
