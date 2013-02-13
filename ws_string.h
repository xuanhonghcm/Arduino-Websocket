#pragma once

#if defined(ARDUINO)
	#include "platform/arduino/ws_string.h"
#elif defined(ASF)
    #include "platform/ASF/ws_string.h"
#else
	#include "platform/generic/ws_string.h"
#endif
