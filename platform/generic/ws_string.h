#pragma once

#include <iostream>
#include <string.h>

namespace websocket
{

    class String
    {
    public:
        // constructors
        // creates a copy of the initial value.
        // if the initial value is null or invalid, or if memory allocation
        // fails, the string will be marked as invalid (i.e. "if (s)" will
        // be false).
        String(char const* s = "")
            : m_string(s)
        { }

        String(String const& str)
            : m_string(str.m_string)
        { }

        unsigned int length() const
        {
            return m_string.length();
        }

        // creates a copy of the assigned value.  if the value is null or
        // invalid, or if the memory allocation fails, the string will be
        // marked as invalid ("if (s)" will be false).
        String& operator =(String const& rhs)
        {
            m_string = rhs.m_string;
            return *this;
        }

        char const* c_str() const
        {
            return m_string.c_str();
        }

        String& operator += (char c)
        {
            m_string.push_back(c);
            return *this;
        }

	// Performs a lexicographical compare, return value similar to that of strcmp.
        int compareTo(String const& s) const;

        bool equals(String const& s) const { return m_string == s.m_string; }
        unsigned char equals(char const* cstr) const;

        bool equalsIgnoreCase(String const& s) const
        {
            return stricmp(m_string.c_str(), s.m_string.c_str()) == 0;
        }

        bool startsWith(String const& prefix) const;
        bool startsWith(String const& prefix, unsigned int offset) const;
        bool endsWith(String const& suffix) const;

        // character acccess
        char operator [](unsigned int index) const
        {
            return m_string[index];
        }

        char& operator [](unsigned int index)
        {
            return m_string[index];
        }

        friend std::ostream& operator<<(std::ostream& out, String const& s)
        {
            return out << s.m_string;
        }

    private:

        std::string m_string;
    };

} // websocket
