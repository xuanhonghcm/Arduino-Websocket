#pragma once

#if defined(ARDUINO)
	#include "platform/arduino/ws_random.h"
#elif defined(ASF)
    #include "platform/ASF/ws_random.h"
#else
	#include "platform/generic/ws_random.h"
#endif
