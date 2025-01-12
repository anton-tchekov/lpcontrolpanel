#include "button.h"
#include "gfx.h"
#include "gui.h"
#include "theme.h"

#define BORDER_SIZE 2

void button_render(Button *b, u32 flags)
{
	u32 inner_color = (flags & (FLAG_SELECTED | FLAG_HOVER)) ?
		THEME_MIDDLE : THEME_DARK;

	u32 border_color = (flags & FLAG_SELECTED) ?
		THEME_LIGHT : THEME_MIDDLE;

	gfx_rect(b->X, b->Y, b->W, b->H, inner_color);
	gfx_rect_border(b->X, b->Y, b->W, b->H, BORDER_SIZE,
			border_color);

	i32 x = b->X + b->W / 2 - gfx_string_width(b->Text) / 2;
	gfx_string(x, b->Y + b->H / 2 - _char_height / 2,
		THEME_LIGHT, b->Text);
}

i32 button_bounds(Button *e, i32 x, i32 y)
{
	return x >= e->X && y >= e->Y && x < e->X + e->W && y < e->Y + e->H;
}
