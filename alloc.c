#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>

static size_t _alloc_cnt, _free_cnt;

void _alloc_fail(size_t size)
{
	fprintf(stderr, "Memory allocation failure (%zu bytes)\n", size);
	exit(1);
}

void *_malloc(size_t size)
{
	void *p = malloc(size);
	if(!p) { _alloc_fail(size); }
	++_alloc_cnt;
	return p;
}

void *_calloc(size_t count, size_t size)
{
	void *p = calloc(count, size);
	if(!p) { _alloc_fail(size); }
	++_alloc_cnt;
	return p;
}

void *_realloc(void *p, size_t size)
{
	if(!p)
	{
		++_alloc_cnt;
	}

	p = realloc(p, size);
	if(!p) { _alloc_fail(size); }
	return p;
}

void _free(void *p)
{
	if(p)
	{
		free(p);
		++_free_cnt;
	}
}

void _alloc_report(void)
{
	printf("%zu allocs, %zu frees\n", _alloc_cnt, _free_cnt);
}
