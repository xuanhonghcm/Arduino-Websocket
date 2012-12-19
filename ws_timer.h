#pragma once

#if defined(ARDUINO)
	#include "platform/arduino/ws_timer.h"
#elif defined(_WIN32)
	#include "platform/win32/ws_timer.h"
#else
	#include "platform/generic/ws_timer.h"
#endif
