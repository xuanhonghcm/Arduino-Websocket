#include "workaround.h"
#include <windows.h>
#include <stdio.h>

int analogRead(uint8_t i)
{
    return 0;
}


void randomSeed(unsigned int)
{

}

long random()
{
    return 0x3132; // TODO: not very random
}

void delay(unsigned long t)
{
    Sleep(t);
}
