#include "textbox.h"
#include "gfx.h"
#include "gui.h"
#include "theme.h"
#include "util.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define BORDER_SIZE          2
#define INPUT_PADDING_X      8
#define INPUT_PADDING_Y      8
#define CURSOR_WIDTH         2
#define CURSOR_HEIGHT         (4 + _char_height)
#define CURSOR_OFFSET_X      0
#define CURSOR_OFFSET_Y      6
#define INPUT_HEIGHT          (17 + _char_height)

static void textbox_replace(Textbox *input, u32 index, u32 count,
	const void *elems, u32 new_count)
{
	u32 new_length = input->Length - count + new_count;
	u32 last_bytes = (input->Length - index - count);

	memmove(input->Text + index + new_count,
		input->Text + index + count,
		last_bytes);

	memcpy(input->Text + index, elems, new_count);

	input->Length = new_length;
}

static void textbox_remove(Textbox *input, u32 index)
{
	textbox_replace(input, index, 1, NULL, 0);
}

void textbox_render(Textbox *i, u32 flags)
{
	u32 inner_color = (flags & (FLAG_SELECTED | FLAG_HOVER)) ?
		THEME_MIDDLE : THEME_DARK;

	gfx_rect(i->X + BORDER_SIZE,
		i->Y + BORDER_SIZE,
		i->W - 2 * BORDER_SIZE,
		INPUT_HEIGHT - 2 * BORDER_SIZE,
		inner_color);

	gfx_rect_border(i->X, i->Y,
		i->W, INPUT_HEIGHT, BORDER_SIZE,
		flags & FLAG_SELECTED ? THEME_LIGHT : THEME_MIDDLE);

	if(i->Selection == i->Position)
	{
		gfx_string_len(i->X + INPUT_PADDING_X, i->Y + INPUT_PADDING_Y,
			THEME_LIGHT, i->Text, i->Length);
	}
	else
	{
		i32 sel_start = i32_min(i->Selection, i->Position);
		i32 sel_len = i32_max(i->Selection, i->Position) - sel_start;

		gfx_string_len(i->X + INPUT_PADDING_X,
			i->Y + INPUT_PADDING_Y,
			THEME_LIGHT,
			i->Text, sel_start);

		i32 sel_x = gfx_string_width_len(i->Text, sel_start);

		gfx_rect(i->X + INPUT_PADDING_X + sel_x, i->Y + CURSOR_OFFSET_Y,
			gfx_string_width_len(i->Text + sel_start, sel_len),
			CURSOR_HEIGHT,
			THEME_LIGHT);

		gfx_string_len(i->X + INPUT_PADDING_X + sel_x, i->Y + INPUT_PADDING_Y,
			THEME_DARK,
			i->Text + sel_start, sel_len);

		gfx_string_len(i->X + INPUT_PADDING_X +
			gfx_string_width_len(i->Text, sel_start + sel_len),
			i->Y + INPUT_PADDING_Y,
			THEME_LIGHT,
			i->Text + sel_start + sel_len,
			i->Length - sel_start - sel_len);
	}

	if(flags & FLAG_SELECTED)
	{
		gfx_rect(i->X + INPUT_PADDING_X +
			gfx_string_width_len(i->Text, i->Position) +
			CURSOR_OFFSET_X,
			i->Y + CURSOR_OFFSET_Y,
			CURSOR_WIDTH,
			CURSOR_HEIGHT,
			THEME_LIGHT);
	}
}

void textbox_clear(Textbox *i)
{
	i->Length = 0;
	i->Selection = 0;
	i->Position = 0;
	i->Text[0] = '\0';
}

static void textbox_selection_replace(Textbox *i, const char *str, i32 len)
{
	i32 sel_start = i32_min(i->Selection, i->Position);
	i32 sel_len = i32_max(i->Selection, i->Position) - sel_start;

	i32 w_new = gfx_string_width_len(str, len);
	i32 w_start = gfx_string_width_len(i->Text, sel_start);
	i32 w_end = gfx_string_width_len(i->Text + sel_start + sel_len,
		i->Length - sel_start - sel_len);

	if(w_new + w_start + w_end >= i->W - 2 * INPUT_PADDING_X)
	{
		return;
	}

	textbox_replace(i, sel_start, sel_len, str, len);
	i->Position = sel_start + len;
	i->Selection = i->Position;
}

static void textbox_left(Textbox *i)
{
	if(i->Selection != i->Position)
	{
		i->Selection = i->Position;
	}
	else if(i->Position > 0)
	{
		--i->Position;
		i->Selection = i->Position;
	}
}

static void textbox_select_left(Textbox *i)
{
	if(i->Position > 0)
	{
		--i->Position;
	}
}

static void textbox_right(Textbox *i)
{
	if(i->Selection != i->Position)
	{
		i->Selection = i->Position;
	}
	else if(i->Position < (i32)i->Length)
	{
		++i->Position;
		i->Selection = i->Position;
	}
}

static void textbox_select_right(Textbox *i)
{
	if(i->Position < (i32)i->Length)
	{
		++i->Position;
	}
}

static void textbox_backspace(Textbox *i)
{
	if(i->Selection != i->Position)
	{
		textbox_selection_replace(i, NULL, 0);
	}
	else if(i->Position > 0)
	{
		--i->Position;
		i->Selection = i->Position;
		textbox_remove(i, i->Position);
	}
}

static void textbox_delete(Textbox *i)
{
	if(i->Selection != i->Position)
	{
		textbox_selection_replace(i, NULL, 0);
	}
	else if(i->Position < (i32)i->Length)
	{
		textbox_remove(i, i->Position);
	}
}

static void textbox_home(Textbox *i)
{
	i->Selection = 0;
	i->Position = 0;
}

static void textbox_select_home(Textbox *i)
{
	i->Position = 0;
}

static void textbox_end(Textbox *i)
{
	i->Position = i->Length;
	i->Selection = i->Position;
}

static void textbox_select_end(Textbox *i)
{
	i->Position = i->Length;
}

static void textbox_char(Textbox *i, u32 chr)
{
	char ins = chr;
	textbox_selection_replace(i, &ins, 1);
}

static void textbox_select_all(Textbox *i)
{
	i->Selection = 0;
	i->Position = i->Length;
}

static void _selection_save(Textbox *i)
{
	i32 sel_start = i32_min(i->Selection, i->Position);
	i32 sel_len = i32_max(i->Selection, i->Position) - sel_start;
	char *p = i->Text + sel_start + sel_len;
	i32 chr = *p;
	*p = '\0';
	SDL_SetClipboardText(i->Text + sel_start);
	*p = chr;
}

static void textbox_copy(Textbox *i)
{
	_selection_save(i);
}

static void textbox_cut(Textbox *i)
{
	_selection_save(i);
	textbox_selection_replace(i, NULL, 0);
}

static void textbox_paste(Textbox *i)
{
	char *p = SDL_GetClipboardText();
	textbox_selection_replace(i, p, strlen(p));
	free(p);
}

void textbox_click(Textbox *i, i32 x)
{
	i32 offset = x - i->X - INPUT_PADDING_X;
	if(offset < 0) { return; }
	offset = (offset + (_char_width / 2)) / _char_width;
	if(offset > i->Length)
	{
		offset = i->Length;
	}

	i->Position = offset;
	i->Selection = offset;
}

void textbox_event_key(Textbox *i, u32 key, u32 chr)
{
	u32 nomods = key & 0xFF;
	if(key == SDL_SCANCODE_HOME)
	{
		textbox_home(i);
	}
	else if(key == (SDL_SCANCODE_HOME | MOD_SHIFT))
	{
		textbox_select_home(i);
	}
	else if(key == SDL_SCANCODE_END)
	{
		textbox_end(i);
	}
	else if(key == (SDL_SCANCODE_END | MOD_SHIFT))
	{
		textbox_select_end(i);
	}
	else if(key == SDL_SCANCODE_LEFT)
	{
		textbox_left(i);
	}
	else if(key == (SDL_SCANCODE_LEFT | MOD_SHIFT))
	{
		textbox_select_left(i);
	}
	else if(key == SDL_SCANCODE_RIGHT)
	{
		textbox_right(i);
	}
	else if(key == (SDL_SCANCODE_RIGHT | MOD_SHIFT))
	{
		textbox_select_right(i);
	}
	else if(nomods == SDL_SCANCODE_BACKSPACE)
	{
		textbox_backspace(i);
	}
	else if(nomods == SDL_SCANCODE_DELETE)
	{
		textbox_delete(i);
	}
	else if(key == (SDL_SCANCODE_A | MOD_CTRL))
	{
		textbox_select_all(i);
	}
	else if(key == (SDL_SCANCODE_C | MOD_CTRL))
	{
		textbox_copy(i);
	}
	else if(key == (SDL_SCANCODE_X | MOD_CTRL))
	{
		textbox_cut(i);
	}
	else if(key == (SDL_SCANCODE_V | MOD_CTRL))
	{
		textbox_paste(i);
	}
	else if(nomods == SDL_SCANCODE_RETURN)
	{
		if(i->Enter)
		{
			i->Enter();
		}
	}
	else if(isprint(chr))
	{
		textbox_char(i, chr);
	}

	i->Text[i->Length] = '\0';
}

i32 textbox_bounds(Textbox *e, i32 x, i32 y)
{
	return x >= e->X && y >= e->Y && x < e->X + e->W && y < e->Y + INPUT_HEIGHT;
}
