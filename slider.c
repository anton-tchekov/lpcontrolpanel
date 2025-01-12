#include "slider.h"
#include "gfx.h"
#include "theme.h"
#include "gui.h"

#define SLIDER_BAR_HEIGHT 10
#define SLIDER_HEIGHT     38
#define SLIDER_KNOB_WIDTH 10

void slider_render(Slider *slider, u32 flags)
{
	gfx_rect(slider->X, slider->Y + SLIDER_HEIGHT / 2 - SLIDER_BAR_HEIGHT / 2,
		slider->W, SLIDER_BAR_HEIGHT,
		flags & FLAG_HOVER ? THEME_MIDDLE : THEME_DARK);

	gfx_rect(slider->X + slider->Value, slider->Y,
		SLIDER_KNOB_WIDTH, SLIDER_HEIGHT, THEME_LIGHT);
}

void slider_click(Slider *slider, i32 x)
{
	slider->Value = x - slider->X;
	if(slider->OnChange)
	{
		slider->OnChange();
	}
}

i32 slider_bounds(Slider *s, i32 x, i32 y)
{
	return x >= s->X && y >= s->Y && x < s->X + s->W && y < s->Y + SLIDER_HEIGHT;
}
