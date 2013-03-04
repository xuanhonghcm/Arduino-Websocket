## Websocket Client and Server

This is a simple library that implements a Websocket client and server running
on an Arduino, windows, and possibly other platforms. The server part has not
evolved with client as much as it could have and probably does not work very 
well. If there is interest in it, then patches are welcome.

### Getting started (Arduino)

The example WebSocketServer.html file should be served from any web server you
have access to. Remember to change the  URL in it to your Arduino.

Install the library to "libraries" folder in your Arduino sketchbook folder.
For example, on a mac that's `~/Documents/Arduino/libraries`.

Try the examples to ensure that things work.

Start playing with your own code!

### Getting started (anything else)

At this time there are no makefiles or any other build scripts to build the
library. Simply add the source files of this library to your project and build.
On some platforms features required by this lib may be missing (e.g. random,
socket). To get around this, you need to implement the functions with
signatures defined in ws_socket.h or ws_random.h. In order to provide your own
implementation you need to define HAVE_LIBWEBSOCKET_RANDOM for random and 
HAVE_LIBWEBSOCKET_SOCKET for socket. You can do this even if this library has
an implementation of a feature for your platform.

### License
GPL See http://www.gnu.org/licenses/gpl.txt.


SHA-1 and Base64 used here are probably public domain (not sure since the
origin did not have any license info). See https://github.com/brandenhall/Arduino-Websocket
It came from there. License info is missing (at the time of this writing).

### Notes

Because of limitations of the current Arduino platform (Uno, Mega, Due at the
time of this writing), this library does not support messages larger than 65535
characters. In addition, this library only supports single-frame text frames.
It currently does not recognize continuation frames, binary frames, or
ping/pong frames.

### Credits
Thank you to anyone that contibuted to the original version of this 
library. This library has evolved to something quite different from the
original and it is probably unfair to continue using the same name for
the library. Will change it if there are any objections.
