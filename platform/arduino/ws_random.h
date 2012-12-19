#pragma once

#include <Arduino.h>

namespace websocket
{
	inline void randomSeed()
	{
		::randomSeed(analogRead(0));
	}

	using ::random;
}
