#include "label.h"
#include "gfx.h"
#include "theme.h"

static i32 label_offset(Label *l)
{
	if(l->Align == ALIGN_CENTER)
	{
		return gfx_string_width(l->Text) / 2;
	}
	else if(l->Align == ALIGN_RIGHT)
	{
		return gfx_string_width(l->Text);
	}

	return 0;
}

void label_render(Label *l)
{
	gfx_string(l->X - label_offset(l), l->Y, THEME_LIGHT, l->Text);
}
