#ifndef _SBI_H
#define _SBI_H

#include "stdint.h"
#include <sbi_ecall_interface.h>

extern usize sbi_call(usize eid, usize fid, usize a0, usize a1, usize a2);

#endif
