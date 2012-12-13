#include "WString.h"

String::String(const char *cstr)
    : m_string(cstr)
{
}


String& String::operator = (const String &rhs)
{
    if (this == &rhs) return *this;

    this->m_string = rhs.m_string;

    return *this;
}

String::~String(void)
{

}

char const* String::c_str() const
{
    return m_string.c_str();
}


unsigned char String::equalsIgnoreCase(const String &s) const
{
    return (stricmp(m_string.c_str(), s.m_string.c_str()) == 0)
            ? 1     //  Are equal, case insensitive
            : 0;
}

unsigned char String::equals(const String &s) const
{
    return m_string.length() == s.m_string.length() && m_string.compare(s.m_string) == 0;
}


char String::operator [] (unsigned int index) const
{
    if (index >= m_string.length())
        return 0;

    return m_string[index];
}


unsigned char String::concat(char c)
{
    m_string.push_back(c);
    return 1;
}

void String::getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index) const
{
    if (!bufsize || !buf) return;
    if (index >= m_string.length())
    {
        buf[0] = 0;
        return;
    }

    unsigned int n = bufsize - 1;
    if (n > m_string.length() - index)
        n = m_string.length() - index;
    strncpy((char *)buf, m_string.c_str() + index, n);
    buf[n] = 0;
}
