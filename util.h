#ifndef __UTIL_H__
#define __UTIL_H__

#include "types.h"

#define ARRLEN(X) (sizeof(X) / sizeof(*X))

i32 i32_min(i32 a, i32 b);
i32 i32_max(i32 a, i32 b);

#endif
