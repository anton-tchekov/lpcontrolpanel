#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "types.h"

typedef struct BUTTON
{
	i32 X, Y, W, H;
	char *Text;
	void (*Click)(void);
} Button;

void button_render(Button *button, u32 flags);
i32 button_bounds(Button *e, i32 x, i32 y);

#endif
