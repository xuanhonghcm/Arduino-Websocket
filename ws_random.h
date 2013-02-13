#pragma once

#if defined(HAVE_LIBWEBSOCKET_RANDOM)
    namespace websocket
    {
        void randomSeed();
        uint32_t random();
    }
#endif

#if !defined(HAVE_LIBWEBSOCKET_RANDOM) && defined(ARDUINO)
    #define HAVE_LIBWEBSOCKET_RANDOM
    #include "platform/arduino/ws_random.h"
#endif

#if !defined(!defined(HAVE_LIBWEBSOCKET_RANDOM))
    #define HAVE_LIBWEBSOCKET_RANDOM
    #include "platform/generic/ws_random.h"
#endif
