#pragma once

#if defined(ARDUINO)
	#include "platform/arduino/ws_debug.h"
#else
	#include "platform/generic/ws_debug.h"
#endif

#if !defined(WEBSOCKET_DEBUG)
	#define WEBSOCKET_DEBUG 0
#endif

#define wsDebug() if (WEBSOCKET_DEBUG) debug_stream
