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
#pragma once

#include "ws_debug.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <malloc.h>
#include <utility>

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
        {
            assert(s != 0);
            m_string = strdup(s);
            m_length = m_capacity = strlen(s);
            assert(m_length <= m_capacity);
        }

        String(String const& str)
            : m_string(0)
            , m_length(0)
            , m_capacity(0)
        {
            *this = str;
            assert(m_length <= m_capacity);
        }

        String(String&& str)
        {
            *this = std::move(str);
            assert(m_length <= m_capacity);
        }

        ~String()
        {
            release();
        }

        unsigned int length() const
        {
            assert(m_length <= m_capacity);
            return m_length;
        }

        // creates a copy of the assigned value.  if the value is null or
        // invalid, or if the memory allocation fails, the string will be
        // marked as invalid ("if (s)" will be false).
        String& operator =(String const& rhs)
        {
            assert(m_length <= m_capacity);
            if (this != &rhs)
            {
                release();
                m_string = strdup(rhs.m_string);
                if (m_string != 0)
                    m_length = m_capacity = strlen(m_string);
            }
            assert(m_length <= m_capacity);
            return *this;
        }

        String& operator =(String&& rhs)
        {
            assert(m_length <= m_capacity);
            if (this != &rhs)
            {
                std::swap(m_string, rhs.m_string);
                std::swap(m_length, rhs.m_length);
                std::swap(m_capacity, rhs.m_capacity);
            }

            assert(m_length <= m_capacity);
            return *this;
        }

        char const* c_str() const
        {
            assert(m_length <= m_capacity);
            assert(m_string != 0);
            return m_string;
        }

        String& operator += (char c)
        {
            assert(m_length <= m_capacity);
            bool const grew = grow();
            assert(grew);

            m_string[m_length++] = c;
            m_string[m_length] = 0;
            return *this;
        }

        // Performs a lexicographical compare, return value similar to that of strcmp.
        int compareTo(String const& s) const;

        bool operator==(String const& rhs) const
        {
            assert(m_length <= m_capacity);
            if (m_length != rhs.m_length)
                return false;

            return strncmp(m_string, rhs.m_string, m_length) == 0;
        }

        bool operator!=(String const& rhs) const
        {
            assert(m_length <= m_capacity);
            return !(*this == rhs);
        }

        bool equalsIgnoreCase(String const& s) const
        {
            assert(m_length <= m_capacity);
            if (m_length != s.m_length)
                return false;

            return strnicmp(m_string, s.m_string, m_length) == 0;
        }

        bool startsWith(String const& prefix) const
        {
            assert(m_length <= m_capacity);
            if (prefix.length() > length())
                return false;

            return strncmp(m_string, prefix.m_string, prefix.length()) == 0;
        }

        bool endsWith(String const& suffix) const
        {
            assert(m_length <= m_capacity);
            if (suffix.length() > length())
                return false;

            return strcmp(c_str() + length() - suffix.length(), suffix.c_str()) == 0;
        }

        // character access
        char operator [](unsigned int index) const
        {
            assert(m_length <= m_capacity);
            assert(index < m_length);
            return m_string[index];
        }

        char& operator [](unsigned int index)
        {
            assert(m_length <= m_capacity);
            assert(index < m_length);
            return m_string[index];
        }

        friend Stream& operator<<(Stream& out, String const& s)
        {
            if (s.m_string != nullptr)
                out << s.c_str();
            return out;
        }

    private:

        void release()
        {
            assert(m_length <= m_capacity);
            if (m_string != 0)
            {
                free(m_string);
                m_string = nullptr;
                m_length = m_capacity = 0;
            }
        }

        /// @retval false - failed to grow.
        bool grow()
        {
            assert(m_length <= m_capacity);
            if (m_length >= m_capacity)
            {
                size_t newCapacity = 10 + m_capacity * 4 / 3;
                char* tmp = static_cast<char*>(realloc(m_string, newCapacity));
                if (tmp == nullptr)
                    return false;

                m_string = tmp;
                m_capacity = newCapacity;
            }
            assert(m_length <= m_capacity);
            return true;
        }

        char* m_string;
        size_t m_length;
        size_t m_capacity;
    };

} // websocket
