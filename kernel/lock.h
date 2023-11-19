#ifndef _LOCK_H
#define _LOCK_H

#include "stdint.h"

typedef u32 Lock;

void lock_acquire(Lock *lock);
void lock_release(Lock *lock);

#endif
