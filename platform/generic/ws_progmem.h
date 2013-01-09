#pragma once

#include <string.h>
#define PROGRAM_MEMORY(x) x
#define F(x) x
#define copyFromProgramMemory memcpy
#define readByteFromProgramMemory(p) (*(p))