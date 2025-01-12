#ifndef __LAMP_H__
#define __LAMP_H__

#include "types.h"

typedef struct
{
	i32 X, Y;
	u32 Color;
	char *Top, *Bottom;
} Lamp;

void lamp_render(Lamp *lamp);
void lamp_setrgb(Lamp *lamp);
void lamp_setbrightness(Lamp *lamp);

#endif
