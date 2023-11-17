#ifndef _SBI_H
#define _SBI_H

#include "stdint.h"

extern usize sbi_call(usize eid, usize fid, usize a0);

#endif
