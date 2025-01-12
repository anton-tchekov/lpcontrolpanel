#ifndef __SLIDER_H__
#define __SLIDER_H__

#include "types.h"

typedef struct SLIDER
{
	i32 X, Y, W;
	i32 Value;
	void (*OnChange)(struct SLIDER *slider);
} Slider;

void slider_render(Slider *slider);

#endif
