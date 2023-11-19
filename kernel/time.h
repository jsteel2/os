#ifndef _STIME_H
#define _STIME_H

#include "stdint.h"

void time_init(void *fdt);
extern u64 time_read(void);

extern u32 timebase_frequency;

#endif
