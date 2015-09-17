#include "Arduino.h"

unsigned _stklen = 4096 * 1024;

DPMI_MEMORY_ALL_LOCK(0)

static __attribute__((constructor(101))) void _f_init()
{
	init();
}

static __attribute__((destructor(101))) void _f_final()
{
	final();
}
