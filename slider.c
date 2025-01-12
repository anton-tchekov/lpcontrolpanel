#include "slider.h"
#include "gfx.h"
#include "theme.h"

#define SLIDER_BAR_HEIGHT 30
#define SLIDER_HEIGHT     10
#define SLIDER_KNOB_WIDTH  5

void slider_render(Slider *slider)
{
	gfx_rect(slider->X, slider->Y, slider->W, SLIDER_BAR_HEIGHT, THEME_MIDDLE);
	gfx_rect(slider->X, slider->Y, SLIDER_KNOB_WIDTH, SLIDER_HEIGHT, THEME_LIGHT);
}
