## Websocket Client and Server for Arduino

This is a simple library that implements a Websocket client and server running
on an Arduino.

### Getting started

The example WebSocketServer.html file should be served from any web server you
have access to. Remember to change the  URL in it to your Arduino.

Install the library to "libraries" folder in your Arduino sketchbook folder.
For example, on a mac that's `~/Documents/Arduino/libraries`.

Try the examples to ensure that things work.

Start playing with your own code!

### License
GPL See http://www.gnu.org/licenses/gpl.txt
Also commercial license is. Contact us.

### Notes

Because of limitations of the current Arduino platform (Uno at the time of
this writing), this library does not support messages larger than 65535
characters. In addition, this library only supports single-frame text frames.
It currently does not recognize continuation frames, binary frames, or
ping/pong frames.

### Credits
Thank you to anyone that contibuted to the original version of this 
library. This library has evolved to something quite different from the
original and it is probably unfair to continue using the same name for
the library. Will change it if there are any objections.
