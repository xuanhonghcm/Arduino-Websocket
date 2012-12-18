#pragma once

#if defined(__AVR__)
	#include "platform/avr/ws_progmem.h"
#else
	#include "platform/generic/ws_progmem.h"
#endif
