#pragma once

#if defined(ARDUINO)
	#include "platform/arduino/ws_socket.h"
#elif defined(_WIN32)
	#include "platform/win32/ws_socket.h"
#else
	#include "platform/generic/ws_socket.h"
#endif
