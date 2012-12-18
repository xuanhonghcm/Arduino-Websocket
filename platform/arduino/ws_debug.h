#pragma once

#include <Arduino.h>
#include <avr/pgmspace.h>

namespace websocket
{
	struct Stream
	{
	};

	inline Stream& operator<<(Stream& out, int i)
	{
		Serial.print(i);
		return out;
	}

	inline Stream& operator<<(Stream& out, char const* s)
	{
		Serial.print(s);
		return out;
	}

	inline Stream& operator<<(Stream& out, char ch)
	{
		Serial.print(ch);
		return out;
	}

	inline Stream& operator<<(Stream& out, __FlashStringHelper const* s)
	{
		const char PROGMEM *p = (const char PROGMEM *)s;
		while (unsigned char c = pgm_read_byte(p++))
		{
			out << c;
		}
		return out;
	}

	// TODO: overload more operators as needed.

	static Stream debug;
}

#define debug_stream websocket::debug
