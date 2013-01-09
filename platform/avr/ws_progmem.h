#pragma once

#include <avr/io.h>
#include <avr/pgmspace.h>
#define PROGRAM_MEMORY(x) x PROGMEM
#define copyFromProgramMemory memcpy_P
#define readByteFromProgramMemory pgm_read_byte