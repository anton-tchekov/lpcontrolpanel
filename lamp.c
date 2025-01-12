#include "lamp.h"
#include "gfx.h"
#include "theme.h"
#include "gui.h"

#define LAMP_SIZE 40

void lamp_render(Lamp *lamp, u32 flags)
{
	gfx_rect(lamp->X - LAMP_SIZE / 2, lamp->Y - LAMP_SIZE / 2,
		LAMP_SIZE, LAMP_SIZE, lamp->Color);
	gfx_rect_border(lamp->X - LAMP_SIZE / 2, lamp->Y - LAMP_SIZE / 2,
		LAMP_SIZE, LAMP_SIZE, 2,
		flags & FLAG_HOVER ? THEME_LIGHT : THEME_MIDDLE);
}

i32 lamp_bounds(Lamp *l, i32 x, i32 y)
{
	return x >= l->X - LAMP_SIZE / 2 &&
		y >= l->Y - LAMP_SIZE / 2 &&
		x < l->X + LAMP_SIZE / 2 &&
		y < l->Y + LAMP_SIZE / 2;
}

void lamp_click(Lamp *lamp)
{
	if(lamp->Click)
	{
		lamp->Click();
	}
}
