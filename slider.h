#ifndef __SLIDER_H__
#define __SLIDER_H__

#include "types.h"

typedef struct SLIDER
{
	i32 X, Y, W;
	i32 Value;
	void (*OnChange)(void);
} Slider;

void slider_render(Slider *slider, u32 flags);
void slider_click(Slider *slider, i32 x);
i32 slider_bounds(Slider *s, i32 x, i32 y);

#endif
