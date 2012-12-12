#ifndef WEBSOCKET_CONTRIB_WORKAROUND_H
#define WEBSOCKET_CONTRIB_WORKAROUND_H

#include <stdint.h>

#  define F(x) x

void randomSeed(unsigned int);
int analogRead(uint8_t i);
long random();
void delay(unsigned long t);


#endif  // WEBSOCKET_CONTRIB_WORKAROUND_H
