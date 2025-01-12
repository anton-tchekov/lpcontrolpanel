#ifndef __ALLOC_H__
#define __ALLOC_H__

#include "types.h"

void _alloc_fail(size_t size);
void *_malloc(size_t size);
void *_calloc(size_t count, size_t size);
void *_realloc(void *p, size_t size);
void _free(void *p);
void _alloc_report(void);

#endif
