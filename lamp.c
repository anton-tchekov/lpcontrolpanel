#include "lamp.h"
#include "gfx.h"
#include "theme.h"
#include "gui.h"

#define LAMP_SIZE 80

void lamp_render(Lamp *lamp, u32 flags)
{
	gfx_rect(lamp->X - LAMP_SIZE / 2, lamp->Y - LAMP_SIZE / 2,
		LAMP_SIZE, LAMP_SIZE, lamp->Color);
	gfx_rect_border(lamp->X - LAMP_SIZE / 2, lamp->Y - LAMP_SIZE / 2,
		LAMP_SIZE, LAMP_SIZE, 2,
		flags & FLAG_HOVER ? THEME_LIGHT : THEME_MIDDLE);
	
	gfx_string(lamp->X - gfx_string_width(lamp->Top) / 2,
		lamp->Y - LAMP_SIZE / 2 - 25,
		THEME_LIGHT, lamp->Top);
	
	gfx_string(lamp->X - gfx_string_width(lamp->Bottom) / 2,
		lamp->Y + LAMP_SIZE / 2,
		THEME_LIGHT, lamp->Bottom);
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
