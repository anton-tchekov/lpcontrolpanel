#ifndef __LAMP_H__
#define __LAMP_H__

#include "types.h"

typedef struct
{
	i32 X, Y;
	u32 Color;
	char *Topic;
	char *Top, *Bottom;
	void (*Click)(void);
} Lamp;

void lamp_render(Lamp *lamp, u32 flags);
void lamp_setrgb(Lamp *lamp);
i32 lamp_bounds(Lamp *lamp, i32 x, i32 y);
void lamp_click(Lamp *lamp);
void lamp_setbrightness(Lamp *lamp);

#endif
