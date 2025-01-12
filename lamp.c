#include "lamp.h"
#include "gfx.h"
#include "theme.h"

#define LAMP_SIZE 40

void lamp_render(Lamp *lamp)
{
	gfx_rect(lamp->X - LAMP_SIZE / 2, lamp->Y - LAMP_SIZE / 2,
		LAMP_SIZE, LAMP_SIZE, lamp->Color);
	gfx_rect_border(lamp->X - LAMP_SIZE / 2, lamp->Y - LAMP_SIZE / 2,
		LAMP_SIZE, LAMP_SIZE, 2, THEME_LIGHT);
}
