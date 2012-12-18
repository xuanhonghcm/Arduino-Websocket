#pragma once

namespace websocket
{
	struct Stream
	{
	};

	Stream& operator<<(Stream& out, int i)
	{
		Serial.print(i);
		return out;
	}

	Stream& operator<<(Stream& out, char const* s)
	{
		Serial.print(s);
		return out;
	}

	Stream& operator<<(Stream& out, ::String const& s)
	{
		struct X : String { char const* c_str() const { return buffer; } };   
		char const* s = static_cast<X const&>(str).c_str();
		return out << s;
	}

	// TODO: overload more operators as needed.
}

#define debug_stream websocket::Stream()
