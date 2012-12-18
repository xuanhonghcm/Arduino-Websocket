#pragma once

#include "Wstring.h"
#include "ws_debug.h"

namespace websocket
{
    class String : public ::String
    {
    public:
        String(char const* s = "") : ::String(s)
	{ }

        char const* c_str() const
	{
		return buffer;
	}

	String& operator=(char const* rhs)
	{
		::String::operator=(rhs);
		return *this;
	}

	String& operator=(::String const& rhs)
	{
		::String::operator=(rhs);
		return *this;
	}

	String& operator=(String const& rhs)
	{
		::String::operator=(rhs);
		return *this;
	}

	friend Stream& operator<<(Stream& out, websocket::String const& s)
	{
		return out << s.c_str();
	}

    };

} // namespace websocket

